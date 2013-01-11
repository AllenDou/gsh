#include "gsh.h"

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#include <ucontext.h>
#endif /* HAVE_BACKTRACE */

#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/uio.h>
#include <limits.h>
#include <float.h>
#include <math.h>
#include <pthread.h>
#include <sys/resource.h>

/* Our shared "common" objects */

struct sharedObjectsStruct shared;

/* Global vars that are actually used as constants. The following double
 * values are used for double on-disk serialization, and are initialized
 * at runtime to avoid strange compiler optimizations. */

double R_Zero, R_PosInf, R_NegInf, R_Nan;

/*================================= Globals ================================= */

/* Global vars */
struct redisServer server; /* server global state */
struct redisCommand *commandTable;
struct redisCommand readonlyCommandTable[] = {
		{"get",	getCommand,	2,0},
		{"set",	grunCommand,3,0},
		{"grun",grunCommand,3,0},
		{"load",loadCommand,3,0},
		{"info",infoCommand,1,0}
};

/*============================ Utility functions ============================ */

void redisLog(int level, const char *fmt, ...) {
		const int syslogLevelMap[] = { LOG_DEBUG, LOG_INFO, LOG_NOTICE, LOG_WARNING };
		const char *c = ".-*#";
		time_t now = time(NULL);
		va_list ap;
		FILE *fp;
		char buf[64];
		char msg[REDIS_MAX_LOGMSG_LEN];

		if (level < server.verbosity) return;

		fp = (server.logfile == NULL) ? stdout : fopen(server.logfile,"a");
		if (!fp) return;

		va_start(ap, fmt);
		vsnprintf(msg, sizeof(msg), fmt, ap);
		va_end(ap);

		strftime(buf,sizeof(buf),"%d %b %H:%M:%S",localtime(&now));
		fprintf(fp,"[%d] %s %c %s\n",(int)getpid(),buf,c[level],msg);
		fflush(fp);

		if (server.logfile) fclose(fp);

		if (server.syslog_enabled) syslog(syslogLevelMap[level], "%s", msg);
}

/* Redis generally does not try to recover from out of memory conditions
 * when allocating objects or strings, it is not clear if it will be possible
 * to report this condition to the client since the networking layer itself
 * is based on heap allocation for send buffers, so we simply abort.
 * At least the code will be simpler to read... */
void oom(const char *msg) {
		redisLog(REDIS_WARNING, "%s: Out of memory\n",msg);
		sleep(1);
		abort();
}

/*====================== Hash table type implementation  ==================== */

/* This is an hash table type that uses the SDS dynamic strings libary as
 * keys and radis objects as values (objects can hold SDS strings,
 * lists, sets). */

int dictSdsKeyCompare(void *privdata, const void *key1,
				const void *key2)
{
		int l1,l2;
		DICT_NOTUSED(privdata);

		l1 = sdslen((sds)key1);
		l2 = sdslen((sds)key2);
		if (l1 != l2) return 0;
		return memcmp(key1, key2, l1) == 0;
}

/* A case insensitive version used for the command lookup table. */
int dictSdsKeyCaseCompare(void *privdata, const void *key1,
				const void *key2)
{
		DICT_NOTUSED(privdata);

		return strcasecmp(key1, key2) == 0;
}

void dictRedisObjectDestructor(void *privdata, void *val)
{
		DICT_NOTUSED(privdata);

		if (val == NULL) return; /* Values of swapped out keys as set to NULL */
		decrRefCount(val);
}

void dictSdsDestructor(void *privdata, void *val)
{
		DICT_NOTUSED(privdata);

		sdsfree(val);
}


unsigned int dictSdsHash(const void *key) {
		return dictGenHashFunction((unsigned char*)key, sdslen((char*)key));
}

unsigned int dictSdsCaseHash(const void *key) {
		return dictGenCaseHashFunction((unsigned char*)key, sdslen((char*)key));
}


/* Db->dict, keys are sds strings, vals are Redis objects. */
dictType dbDictType = {
		dictSdsHash,                /* hash function */
		NULL,                       /* key dup */
		NULL,                       /* val dup */
		dictSdsKeyCompare,          /* key compare */
		dictSdsDestructor,          /* key destructor */
		dictRedisObjectDestructor   /* val destructor */
};

/* Command table. sds string -> command struct pointer. */
dictType commandTableDictType = {
		dictSdsCaseHash,           /* hash function */
		NULL,                      /* key dup */
		NULL,                      /* val dup */
		dictSdsKeyCaseCompare,     /* key compare */
		dictSdsDestructor,         /* key destructor */
		NULL                       /* val destructor */
};


void updateLRUClock(void) {
		server.lruclock = (time(NULL)/REDIS_LRU_CLOCK_RESOLUTION) &
				REDIS_LRU_CLOCK_MAX;
}

int serverCron(struct aeEventLoop *eventLoop, long long id, void *clientData) {
		int j, loops = server.cronloops;
		REDIS_NOTUSED(eventLoop);
		REDIS_NOTUSED(id);
		REDIS_NOTUSED(clientData);

		/* We take a cached value of the unix time in the global state because
		 * with virtual memory and aging there is to store the current time
		 * in objects at every object access, and accuracy is not needed.
		 * To access a global var is faster than calling time(NULL) */
		server.unixtime = time(NULL);

		/* We have just 22 bits per object for LRU information.
		 * So we use an (eventually wrapping) LRU clock with 10 seconds resolution.
		 * 2^22 bits with 10 seconds resoluton is more or less 1.5 years.
		 *
		 * Note that even if this will wrap after 1.5 years it's not a problem,
		 * everything will still work but just some object will appear younger
		 * to Redis. But for this to happen a given object should never be touched
		 * for 1.5 years.
		 *
		 * Note that you can change the resolution altering the
		 * REDIS_LRU_CLOCK_RESOLUTION define.
		 */
		updateLRUClock();

		/* Record the max memory used since the server was started. */
		if (zmalloc_used_memory() > server.stat_peak_memory)
				server.stat_peak_memory = zmalloc_used_memory();

		/* We received a SIGTERM, shutting down here in a safe way, as it is
		 * not ok doing so inside the signal handler. */
		if (server.shutdown_asap) {
				if (prepareForShutdown() == REDIS_OK) exit(0);
				redisLog(REDIS_WARNING,"SIGTERM received but errors trying to shut down the server, check the logs for more information");
		}

		/* Show some info about non-empty databases */
		for (j = 0; j < server.dbnum; j++) {
				long long size, used ;

				size = dictSlots(server.db[j].dict);
				used = dictSize(server.db[j].dict);
				if (!(loops % 50) && (used )) {
						redisLog(REDIS_VERBOSE,"DB %d: %lld keys in %lld slots HT.",j,used ,size);
						/* dictPrintStats(server.dict); */
				}
		}

		/* We don't want to resize the hash tables while a bacground saving
		 * is in progress: the saving child is created using fork() that is
		 * implemented with a copy-on-write semantic in most modern systems, so
		 * if we resize the HT while there is the saving child at work actually
		 * a lot of memory movements in the parent will cause a lot of pages
		 * copied. */

		/* Close connections of timedout clients */
		if ((server.maxidletime && !(loops % 100)))
				closeTimedoutClients();

		server.cronloops++;
		return 100;
}

/* =========================== Server initialization ======================== */

void createSharedObjects(void) {
		int j;

		shared.crlf = createObject(REDIS_STRING,sdsnew("\r\n"));
		shared.ok = createObject(REDIS_STRING,sdsnew("+OK\r\n"));
		shared.err = createObject(REDIS_STRING,sdsnew("-ERR\r\n"));
		shared.emptybulk = createObject(REDIS_STRING,sdsnew("$0\r\n\r\n"));
		shared.czero = createObject(REDIS_STRING,sdsnew(":0\r\n"));
		shared.cone = createObject(REDIS_STRING,sdsnew(":1\r\n"));
		shared.cnegone = createObject(REDIS_STRING,sdsnew(":-1\r\n"));
		shared.nullbulk = createObject(REDIS_STRING,sdsnew("$-1\r\n"));
		shared.nullmultibulk = createObject(REDIS_STRING,sdsnew("*-1\r\n"));
		shared.emptymultibulk = createObject(REDIS_STRING,sdsnew("*0\r\n"));
		shared.pong = createObject(REDIS_STRING,sdsnew("+PONG\r\n"));
		shared.queued = createObject(REDIS_STRING,sdsnew("+QUEUED\r\n"));
		shared.wrongtypeerr = createObject(REDIS_STRING,sdsnew(
								"-ERR Operation against a key holding the wrong kind of value\r\n"));
		shared.nokeyerr = createObject(REDIS_STRING,sdsnew(
								"-ERR no such key\r\n"));
		shared.syntaxerr = createObject(REDIS_STRING,sdsnew(
								"-ERR syntax error\r\n"));
		shared.sameobjecterr = createObject(REDIS_STRING,sdsnew(
								"-ERR source and destination objects are the same\r\n"));
		shared.outofrangeerr = createObject(REDIS_STRING,sdsnew(
								"-ERR index out of range\r\n"));
		shared.loadingerr = createObject(REDIS_STRING,sdsnew(
								"-LOADING Redis is loading the dataset in memory\r\n"));
		shared.space = createObject(REDIS_STRING,sdsnew(" "));
		shared.colon = createObject(REDIS_STRING,sdsnew(":"));
		shared.plus = createObject(REDIS_STRING,sdsnew("+"));

		for (j = 0; j < REDIS_SHARED_SELECT_CMDS; j++) {
				shared.select[j] = createObject(REDIS_STRING,
								sdscatprintf(sdsempty(),"select %d\r\n", j));
		}
		shared.messagebulk = createStringObject("$7\r\nmessage\r\n",13);
		shared.pmessagebulk = createStringObject("$8\r\npmessage\r\n",14);
		shared.subscribebulk = createStringObject("$9\r\nsubscribe\r\n",15);
		shared.unsubscribebulk = createStringObject("$11\r\nunsubscribe\r\n",18);
		shared.psubscribebulk = createStringObject("$10\r\npsubscribe\r\n",17);
		shared.punsubscribebulk = createStringObject("$12\r\npunsubscribe\r\n",19);
		shared.mbulk3 = createStringObject("*3\r\n",4);
		shared.mbulk4 = createStringObject("*4\r\n",4);
		for (j = 0; j < REDIS_SHARED_INTEGERS; j++) {
				shared.integers[j] = createObject(REDIS_STRING,(void*)(long)j);
				shared.integers[j]->encoding = REDIS_ENCODING_INT;
		}
}

void initServerConfig() {
		server.arch_bits = (sizeof(long) == 8) ? 64 : 32;
		server.port = REDIS_SERVERPORT;
		server.bindaddr = NULL;
		server.ipfd = -1;
		server.dbnum = REDIS_DEFAULT_DBNUM;
		server.verbosity = REDIS_VERBOSE;
		server.maxidletime = REDIS_MAXIDLETIME;
		server.client_max_querybuf_len = REDIS_MAX_QUERYBUF_LEN;
		server.logfile = NULL; /* NULL = log on standard output */
		server.syslog_enabled = 0;
		server.syslog_ident = zstrdup("redis");
		server.syslog_facility = LOG_LOCAL0;
		server.daemonize = 0;
		server.pidfile = zstrdup("/var/run/redis.pid");
		server.maxclients = 0;
		server.shutdown_asap = 0;

		updateLRUClock();

		/* Double constants initialization */
		R_Zero = 0.0;
		R_PosInf = 1.0/R_Zero;
		R_NegInf = -1.0/R_Zero;
		R_Nan = R_Zero/R_Zero;

		/* Command table -- we intiialize it here as it is part of the
		 * initial configuration, since command names may be changed via
		 * redis.conf using the rename-command directive. */
		server.commands = dictCreate(&commandTableDictType,NULL);
		populateCommandTable();
		server.delCommand = lookupCommandByCString("del");
		server.multiCommand = lookupCommandByCString("multi");

		/* Assert */
		server.assert_failed = "<no assertion failed>";
		server.assert_file = "<no file>";
		server.assert_line = 0;
		server.bug_report_start = 0;
		//server.fc_list = 0;
		//server.fc_num = 0;
		//server.formulas = 0;
		//server.fmnum = 0;
		server.fms = dictCreate(&commandTableDictType,NULL);
}

void initServer() {
		int j;

		signal(SIGHUP, SIG_IGN);
		signal(SIGPIPE, SIG_IGN);
		setupSignalHandlers();

		if (server.syslog_enabled) {
				openlog(server.syslog_ident, LOG_PID | LOG_NDELAY | LOG_NOWAIT,
								server.syslog_facility);
		}

		server.mainthread = pthread_self();
		server.current_client = NULL;
		server.clients = listCreate();
		createSharedObjects();
		server.el = aeCreateEventLoop();
		server.db = zmalloc(sizeof(redisDb)*server.dbnum);

		if (server.port != 0) {
				server.ipfd = anetTcpServer(server.neterr,server.port,server.bindaddr);
				if (server.ipfd == ANET_ERR) {
						redisLog(REDIS_WARNING, "Opening port %d: %s",
										server.port, server.neterr);
						exit(1);
				}
		}

		for (j = 0; j < server.dbnum; j++) {
				server.db[j].dict = dictCreate(&dbDictType,NULL);
				server.db[j].id = j;
		}

		server.cronloops = 0;
		server.dirty = 0;
		server.stat_numcommands = 0;
		server.stat_numconnections = 0;
		server.stat_starttime = time(NULL);
		server.stat_peak_memory = 0;
		server.unixtime = time(NULL);
		aeCreateTimeEvent(server.el, 1, serverCron, NULL, NULL);
		if (server.ipfd > 0 && aeCreateFileEvent(server.el,server.ipfd,AE_READABLE,
								acceptTcpHandler,NULL) == AE_ERR) oom("creating file event");

		/* 32 bit instances are limited to 4GB of address space, so if there is
		 * no explicit limit in the user provided configuration we set a limit
		 * at 3.5GB using maxmemory with 'noeviction' policy'. This saves
		 * useless crashes of the Redis instance. */

		srand(time(NULL)^getpid());
		gsh_init();
}

/* Populates the Redis Command Table starting from the hard coded list
 * we have on top of redis.c file. */
void populateCommandTable(void) {
		int j;
		int numcommands = sizeof(readonlyCommandTable)/sizeof(struct redisCommand);

		for (j = 0; j < numcommands; j++) {
				struct redisCommand *c = readonlyCommandTable+j;
				int retval;

				retval = dictAdd(server.commands, sdsnew(c->name), c);
				assert(retval == DICT_OK);
		}
}

/* ====================== Commands lookup and execution ===================== */

struct redisCommand *lookupCommand(sds name) {
		return dictFetchValue(server.commands, name);
}

struct redisCommand *lookupCommandByCString(char *s) {
		struct redisCommand *cmd;
		sds name = sdsnew(s);

		cmd = dictFetchValue(server.commands, name);
		sdsfree(name);
		return cmd;
}

/* Call() is the core of Redis execution of a command */
void call(redisClient *c) {
		long long dirty, start = ustime(), duration;

		dirty = server.dirty;
		c->cmd->proc(c);
		dirty = server.dirty-dirty;
		duration = ustime()-start;
		server.stat_numcommands++;
}

int processCommand(redisClient *c) {
		if (!strcasecmp(c->argv[0]->ptr,"quit")) {
				addReply(c,shared.ok);
				c->flags |= REDIS_CLOSE_AFTER_REPLY;
				return REDIS_ERR;
		}

		/* Now lookup the command and check ASAP about trivial error conditions
		 * such as wrong arity, bad command name and so forth. */
		c->cmd = c->lastcmd = lookupCommand(c->argv[0]->ptr);
		if (!c->cmd) {
				addReplyErrorFormat(c,"unknown command '%s'",
								(char*)c->argv[0]->ptr);
				return REDIS_OK;
		} else if ((c->cmd->arity > 0 && c->cmd->arity != c->argc) ||
						(c->argc < -c->cmd->arity)) {
				addReplyErrorFormat(c,"wrong number of arguments for '%s' command",
								c->cmd->name);
				return REDIS_OK;
		}

		call(c);

		return REDIS_OK;
}

/*================================== Shutdown =============================== */

int prepareForShutdown() {
		redisLog(REDIS_WARNING,"User requested shutdown...");
		if (server.daemonize) {
				redisLog(REDIS_NOTICE,"Removing the pid file.");
				unlink(server.pidfile);
		}
		/* Close the listening sockets. Apparently this allows faster restarts. */
		if (server.ipfd != -1) close(server.ipfd);

		redisLog(REDIS_WARNING,"Redis is now ready to exit, bye bye...");
		return REDIS_OK;
}

/*================================== Commands =============================== */


/* Convert an amount of bytes into a human readable string in the form
 * of 100B, 2G, 100M, 4K, and so forth. */
void bytesToHuman(char *s, unsigned long long n) {
		double d;

		if (n < 1024) {
				/* Bytes */
				sprintf(s,"%lluB",n);
				return;
		} else if (n < (1024*1024)) {
				d = (double)n/(1024);
				sprintf(s,"%.2fK",d);
		} else if (n < (1024LL*1024*1024)) {
				d = (double)n/(1024*1024);
				sprintf(s,"%.2fM",d);
		} else if (n < (1024LL*1024*1024*1024)) {
				d = (double)n/(1024LL*1024*1024);
				sprintf(s,"%.2fG",d);
		}
}

/* Create the string returned by the INFO command. This is decoupled
 * by the INFO command itself as we need to report the same information
 * on memory corruption problems. */
sds genRedisInfoString(void) {
		sds info;
		time_t uptime = time(NULL)-server.stat_starttime;
		char hmem[64], peak_hmem[64];
		struct rusage self_ru, c_ru;
		unsigned long lol, bib;

		getrusage(RUSAGE_SELF, &self_ru);
		getrusage(RUSAGE_CHILDREN, &c_ru);
		getClientsMaxBuffers(&lol,&bib);

		bytesToHuman(hmem,zmalloc_used_memory());
		bytesToHuman(peak_hmem,server.stat_peak_memory);
		info = sdscatprintf(sdsempty(),
						"gsh_version:%s\r\n"
						"arch_bits:%d\r\n"
						"multiplexing_api:%s\r\n"
						"gcc_version:%d.%d.%d\r\n"
						"process_id:%ld\r\n"
						"uptime_in_seconds:%ld\r\n"
						"uptime_in_days:%ld\r\n"
						"lru_clock:%ld\r\n"
						"used_cpu_sys:%.2f\r\n"
						"used_cpu_user:%.2f\r\n"
						"used_cpu_sys_children:%.2f\r\n"
						"used_cpu_user_children:%.2f\r\n"
						"connected_clients:%d\r\n"
						"client_longest_output_list:%lu\r\n"
						"client_biggest_input_buf:%lu\r\n"
						"used_memory:%zu\r\n"
						"used_memory_human:%s\r\n"
						"used_memory_rss:%zu\r\n"
						"used_memory_peak:%zu\r\n"
						"used_memory_peak_human:%s\r\n"
						"mem_fragmentation_ratio:%.2f\r\n"
						"mem_allocator:%s\r\n"
						"changes_since_last_save:%lld\r\n"
						"total_connections_received:%lld\r\n"
						"total_commands_processed:%lld\r\n"
						,REDIS_VERSION,
				server.arch_bits,
				aeGetApiName(),
#ifdef __GNUC__
				__GNUC__,__GNUC_MINOR__,__GNUC_PATCHLEVEL__,
#else
				0,0,0,
#endif
				(long) getpid(),
				uptime,
				uptime/(3600*24),
				(unsigned long) server.lruclock,
				(float)self_ru.ru_stime.tv_sec+(float)self_ru.ru_stime.tv_usec/1000000,
				(float)self_ru.ru_utime.tv_sec+(float)self_ru.ru_utime.tv_usec/1000000,
				(float)c_ru.ru_stime.tv_sec+(float)c_ru.ru_stime.tv_usec/1000000,
				(float)c_ru.ru_utime.tv_sec+(float)c_ru.ru_utime.tv_usec/1000000,
				listLength(server.clients),
				lol, bib,
				zmalloc_used_memory(),
				hmem,
				zmalloc_get_rss(),
				server.stat_peak_memory,
				peak_hmem,
				zmalloc_get_fragmentation_ratio(),
				ZMALLOC_LIB,
				server.dirty,
				server.stat_numconnections,
				server.stat_numcommands
						);

		dictIterator *di;
		dictEntry *de;
		di = dictGetSafeIterator(server.fms);
		int j = 0;
		while((de = dictNext(di)) != NULL) 
		{
				sds key = dictGetEntryKey(de);
				info = sdscatprintf(info, "formulas[%d]=[%s]\r\n",j++,key);
		}
		dictReleaseIterator(di);
		/*int j;
		  for (j = 0; j < server.fmnum; j++) 
		  info = sdscatprintf(info, "formulas[%d]=[%s]\r\n",j,server.formulas[j]);
		  */
		/*for (j = 0; j < server.dbnum; j++) {
		  long long keys;

		  keys = dictSize(server.db[j].dict);
		  if (keys) {
		  info = sdscatprintf(info, "db%d:keys=%lld\r\n",
		  j, keys);
		  }
		  }*/
		return info;
}

void infoCommand(redisClient *c) {
		sds info = genRedisInfoString();
		addReplySds(c,sdscatprintf(sdsempty(),"$%lu\r\n",(unsigned long)sdslen(info)));
		addReplySds(c,info);
		addReply(c,shared.crlf);
}

/* =================================== Main! ================================ */

#ifdef __linux__
int linuxOvercommitMemoryValue(void) {
		FILE *fp = fopen("/proc/sys/vm/overcommit_memory","r");
		char buf[64];

		if (!fp) return -1;
		if (fgets(buf,64,fp) == NULL) {
				fclose(fp);
				return -1;
		}
		fclose(fp);

		return atoi(buf);
}

void linuxOvercommitMemoryWarning(void) {
		if (linuxOvercommitMemoryValue() == 0) {
				redisLog(REDIS_WARNING,"WARNING overcommit_memory is set to 0! Background save may fail under low memory condition. To fix this issue add 'vm.overcommit_memory = 1' to /etc/sysctl.conf and then reboot or run the command 'sysctl vm.overcommit_memory=1' for this to take effect.");
		}
}
#endif /* __linux__ */

void createPidFile(void) {
		/* Try to write the pid file in a best-effort way. */
		FILE *fp = fopen(server.pidfile,"w");
		if (fp) {
				fprintf(fp,"%d\n",(int)getpid());
				fclose(fp);
		}
}

void daemonize(void) {
		int fd;

		if (fork() != 0) exit(0); /* parent exits */
		setsid(); /* create a new session */

		/* Every output goes to /dev/null. If Redis is daemonized but
		 * the 'logfile' is set to 'stdout' in the configuration file
		 * it will not log at all. */
		if ((fd = open("/dev/null", O_RDWR, 0)) != -1) {
				dup2(fd, STDIN_FILENO);
				dup2(fd, STDOUT_FILENO);
				dup2(fd, STDERR_FILENO);
				if (fd > STDERR_FILENO) close(fd);
		}
}

void version() {
		printf("Redis server version %s\n", REDIS_VERSION);
		exit(0);
}

void usage() {
		fprintf(stderr,"Usage: ./redis-server [/path/to/gsh.conf]\n");
		fprintf(stderr,"       ./redis-server - (read config from stdin)\n");
		fprintf(stderr,"       ./redis-server --test-memory <megabytes>\n\n");
		exit(1);
}

int main(int argc, char **argv) {
		time_t start;

		initServerConfig();
		if (argc == 2) {
				if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0) version();
				if (strcmp(argv[1], "--help") == 0) usage();
				loadServerConfig(argv[1]);
		} else if ((argc > 2)) {
				usage();
		} else {
				redisLog(REDIS_WARNING,"Warning: no config file specified, using the default config. In order to specify a config file use 'redis-server /path/to/gsh.conf'");
		}
		if (server.daemonize) daemonize();
		initServer();
		if (server.daemonize) createPidFile();
		redisLog(REDIS_NOTICE,"Server started, Redis version " REDIS_VERSION);

#ifdef __linux__
		linuxOvercommitMemoryWarning();
#endif
		start = time(NULL);
		if (server.ipfd > 0)
				redisLog(REDIS_NOTICE,"The server is now ready to accept connections on port %d", server.port);

		aeMain(server.el);
		aeDeleteEventLoop(server.el);
		return 0;
}

#ifdef HAVE_BACKTRACE
static void *getMcontextEip(ucontext_t *uc) {
#if defined(__FreeBSD__)
		return (void*) uc->uc_mcontext.mc_eip;
#elif defined(__dietlibc__)
		return (void*) uc->uc_mcontext.eip;
#elif defined(__APPLE__) && !defined(MAC_OS_X_VERSION_10_6)

#if __x86_64__
		return (void*) uc->uc_mcontext->__ss.__rip;
#elif __i386__
		return (void*) uc->uc_mcontext->__ss.__eip;
#else
		return (void*) uc->uc_mcontext->__ss.__srr0;
#endif

#elif defined(__APPLE__) && defined(MAC_OS_X_VERSION_10_6)
#if defined(_STRUCT_X86_THREAD_STATE64) && !defined(__i386__)
		return (void*) uc->uc_mcontext->__ss.__rip;
#else
		return (void*) uc->uc_mcontext->__ss.__eip;
#endif

#elif defined(__i386__)
		return (void*) uc->uc_mcontext.gregs[14]; /* Linux 32 */
#elif defined(__X86_64__) || defined(__x86_64__)
		return (void*) uc->uc_mcontext.gregs[16]; /* Linux 64 */
#elif defined(__ia64__) /* Linux IA64 */
		return (void*) uc->uc_mcontext.sc_ip;
#else
		return NULL;
#endif
}

void bugReportStart(void) {
		if (server.bug_report_start == 0) {
				redisLog(REDIS_WARNING,
								"=== REDIS BUG REPORT START: Cut & paste starting from here ===");
				server.bug_report_start = 1;
		}
}

static void sigsegvHandler(int sig, siginfo_t *info, void *secret) {
		void *trace[100];
		char **messages = NULL;
		int i, trace_size = 0;
		ucontext_t *uc = (ucontext_t*) secret;
		sds infostring, clients;
		struct sigaction act;
		REDIS_NOTUSED(info);

		bugReportStart();
		redisLog(REDIS_WARNING,
						"    Redis %s crashed by signal: %d", REDIS_VERSION, sig);
		redisLog(REDIS_WARNING,
						"    Failed assertion: %s (%s:%d)", server.assert_failed,
						server.assert_file, server.assert_line);

		/* Generate the stack trace */
		trace_size = backtrace(trace, 100);

		/* overwrite sigaction with caller's address */
		if (getMcontextEip(uc) != NULL) {
				trace[1] = getMcontextEip(uc);
		}
		messages = backtrace_symbols(trace, trace_size);
		redisLog(REDIS_WARNING, "--- STACK TRACE");
		for (i=1; i<trace_size; ++i)
				redisLog(REDIS_WARNING,"%s", messages[i]);

		/* Log INFO and CLIENT LIST */
		redisLog(REDIS_WARNING, "--- INFO OUTPUT");
		infostring = genRedisInfoString();
		redisLog(REDIS_WARNING, infostring);
		redisLog(REDIS_WARNING, "--- CLIENT LIST OUTPUT");
		clients = getAllClientsInfoString();
		redisLog(REDIS_WARNING, clients);
		/* Don't sdsfree() strings to avoid a crash. Memory may be corrupted. */

		/* Log CURRENT CLIENT info */
		if (server.current_client) {
				redisClient *cc = server.current_client;
				sds client;
				int j;

				redisLog(REDIS_WARNING, "--- CURRENT CLIENT INFO");
				client = getClientInfoString(cc);
				redisLog(REDIS_WARNING,"client: %s", client);
				/* Missing sdsfree(client) to avoid crash if memory is corrupted. */
				for (j = 0; j < cc->argc; j++) {
						robj *decoded;

						decoded = getDecodedObject(cc->argv[j]);
						redisLog(REDIS_WARNING,"argv[%d]: '%s'", j, (char*)decoded->ptr);
						decrRefCount(decoded);
				}
				/* Check if the first argument, usually a key, is found inside the
				 * selected DB, and if so print info about the associated object. */
				if (cc->argc >= 1) {
						robj *val, *key;
						dictEntry *de;

						key = getDecodedObject(cc->argv[1]);
						de = dictFind(cc->db->dict, key->ptr);
						if (de) {
								val = dictGetEntryVal(de);
								redisLog(REDIS_WARNING,"key '%s' found in DB containing the following object:", key->ptr);
								redisLogObjectDebugInfo(val);
						}
						decrRefCount(key);
				}
		}

		redisLog(REDIS_WARNING,
						"=== REDIS BUG REPORT END. Make sure to include from START to END. ===\n\n"
						"       Please report the crash opening an issue on github:\n\n"
						"           http://github.com/antirez/redis/issues\n\n"
						"  Suspect RAM error? Use redis-server --test-memory to veryfy it.\n\n"
				);
		/* free(messages); Don't call free() with possibly corrupted memory. */
		if (server.daemonize) unlink(server.pidfile);

		/* Make sure we exit with the right signal at the end. So for instance
		 * the core will be dumped if enabled. */
		sigemptyset (&act.sa_mask);
		/* When the SA_SIGINFO flag is set in sa_flags then sa_sigaction
		 * is used. Otherwise, sa_handler is used */
		act.sa_flags = SA_NODEFER | SA_ONSTACK | SA_RESETHAND;
		act.sa_handler = SIG_DFL;
		sigaction (sig, &act, NULL);
		kill(getpid(),sig);
}
#endif /* HAVE_BACKTRACE */

static void sigtermHandler(int sig) {
		REDIS_NOTUSED(sig);

		redisLog(REDIS_WARNING,"Received SIGTERM, scheduling shutdown...");
		server.shutdown_asap = 1;
}

void setupSignalHandlers(void) {
		struct sigaction act;

		/* When the SA_SIGINFO flag is set in sa_flags then sa_sigaction is used.
		 * Otherwise, sa_handler is used. */
		sigemptyset(&act.sa_mask);
		act.sa_flags = SA_NODEFER | SA_ONSTACK | SA_RESETHAND;
		act.sa_handler = sigtermHandler;
		sigaction(SIGTERM, &act, NULL);

#ifdef HAVE_BACKTRACE
		sigemptyset(&act.sa_mask);
		act.sa_flags = SA_NODEFER | SA_ONSTACK | SA_RESETHAND | SA_SIGINFO;
		act.sa_sigaction = sigsegvHandler;
		sigaction(SIGSEGV, &act, NULL);
		sigaction(SIGBUS, &act, NULL);
		sigaction(SIGFPE, &act, NULL);
		sigaction(SIGILL, &act, NULL);
#endif
		return;
}



/* The End */
