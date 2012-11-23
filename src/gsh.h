#ifndef __REDIS_H
#define __REDIS_H

#include "common/fmacros.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <unistd.h>
#include <errno.h>
#include <inttypes.h>
#include <pthread.h>
#include <syslog.h>

#include "ae.h"     /* Event driven programming library */
#include "dict.h"   /* Hash tables */
#include "anet.h"   /* Networking the easy way */
#include "config.h"
#include "common/sds.h"    /* Dynamic safe strings */
#include "common/adlist.h" /* Linked lists */
#include "common/zmalloc.h" /* total memory usage aware version of malloc/free */
#include "common/util.h"
#include <dlfcn.h>
/* Error codes */
#define REDIS_OK                0
#define REDIS_ERR               -1

/* Static server configuration */
#define REDIS_VERSION 			"Based on Redis-v2.4.14."
#define REDIS_SERVERPORT        6522    /* TCP port */
#define REDIS_MAXIDLETIME       0       /* default client timeout: infinite */
#define REDIS_MAX_QUERYBUF_LEN  (1024*1024*1024) /* 1GB max query buffer. */
#define REDIS_IOBUF_LEN         (1024*16)
#define REDIS_DEFAULT_DBNUM     16
#define REDIS_CONFIGLINE_MAX    1024
#define REDIS_MAX_WRITE_PER_EVENT (1024*64)
#define REDIS_SHARED_SELECT_CMDS 10
#define REDIS_SHARED_INTEGERS 	10000
#define REDIS_REPLY_CHUNK_BYTES (5*1500) /* 5 TCP packets with default MTU */
#define REDIS_INLINE_MAX_SIZE   (1024*64) /* Max size of inline reads */
#define REDIS_MAX_LOGMSG_LEN    4096 /* Default maximum length of syslog messages */

/* Object types */
#define REDIS_STRING 0

/* Objects encoding. Some kind of objects like Strings and Hashes can be
 * internally represented in multiple ways. The 'encoding' field of the object
 * is set to one of this fields for this object. */
#define REDIS_ENCODING_RAW 0     /* Raw representation */
#define REDIS_ENCODING_INT 1     /* Encoded as integer */

/* Client flags */
#define REDIS_CLOSE_AFTER_REPLY 128 /* Close after writing entire reply. */

/* Client request types */
#define REDIS_REQ_INLINE 1
#define REDIS_REQ_MULTIBULK 2

/* Log levels */
#define REDIS_DEBUG 0
#define REDIS_VERBOSE 1
#define REDIS_NOTICE 2
#define REDIS_WARNING 3

/* Anti-warning macro... */
#define REDIS_NOTUSED(V) ((void) V)

/* We can print the stacktrace, so our assert is defined this way: */
#define redisAssert(_e) ((_e)?(void)0 : (_redisAssert(#_e,__FILE__,__LINE__),_exit(1)))
#define redisPanic(_e) _redisPanic(#_e,__FILE__,__LINE__),_exit(1)
void _redisAssert(char *estr, char *file, int line);
void _redisPanic(char *msg, char *file, int line);
void bugReportStart(void);

/*-----------------------------------------------------------------------------
 * Data types
 *----------------------------------------------------------------------------*/

/* A redis object, that is a type able to hold a string / list / set */

/* The actual Redis Object */
#define REDIS_LRU_CLOCK_MAX ((1<<21)-1) /* Max value of obj->lru */
#define REDIS_LRU_CLOCK_RESOLUTION 10 /* LRU clock resolution in seconds */
typedef struct redisObject {
		unsigned type:4;
		unsigned storage:2;     /* REDIS_VM_MEMORY or REDIS_VM_SWAPPING */
		unsigned encoding:4;
		unsigned lru:22;        /* lru time (relative to server.lruclock) */
		int refcount;
		void *ptr;
		/* VM fields are only allocated if VM is active, otherwise the
		 * object allocation function will just allocate
		 * sizeof(redisObjct) minus sizeof(redisObjectVM), so using
		 * Redis without VM active will not have any overhead. */
} robj;


/* Macro used to initalize a Redis object allocated on the stack.
 * Note that this macro is taken near the structure definition to make sure
 * we'll update it when the structure is changed, to avoid bugs like
 * bug #85 introduced exactly in this way. */
#define initStaticStringObject(_var,_ptr) do { \
		_var.refcount = 1; \
		_var.type = REDIS_STRING; \
		_var.encoding = REDIS_ENCODING_RAW; \
		_var.ptr = _ptr; \
} while(0);

typedef struct redisDb {
		dict *dict;                 /* The keyspace for this DB */
		//    dict *expires;              /* Timeout of keys with a timeout set */
		//    dict *blocking_keys;        /* Keys with clients waiting for data (BLPOP) */
		//    dict *io_keys;              /* Keys with clients waiting for VM I/O */
		//    dict *watched_keys;         /* WATCHED keys for MULTI/EXEC CAS */
		int id;
} redisDb;

typedef struct blockingState {
		robj **keys;            /* The key we are waiting to terminate a blocking
								 * operation such as BLPOP. Otherwise NULL. */
		int count;              /* Number of blocking keys */
		time_t timeout;         /* Blocking operation timeout. If UNIX current time
								 * is >= timeout then the operation timed out. */
		robj *target;           /* The key that should receive the element,
								 * for BRPOPLPUSH. */
} blockingState;

/* With multiplexing we need to take per-clinet state.
 * Clients are taken in a liked list. */
typedef struct redisClient {
		int fd;
		redisDb *db;
		int dictid;
		sds querybuf;
		int argc;
		robj **argv;
		struct redisCommand *cmd, *lastcmd;
		int reqtype;
		int multibulklen;       /* number of multi bulk arguments left to read */
		long bulklen;           /* length of bulk argument in multi bulk request */
		list *reply;
		unsigned long reply_bytes; /* Tot bytes of objects in reply list */
		int sentlen;
		time_t lastinteraction; /* time of the last interaction, used for timeout */
		int flags;              /* REDIS_SLAVE | REDIS_MONITOR | REDIS_MULTI ... */

		/* Response buffer */
		int bufpos;
		char buf[REDIS_REPLY_CHUNK_BYTES];
} redisClient;

struct sharedObjectsStruct {
		robj *crlf, *ok, *err, *emptybulk, *czero, *cone, *cnegone, *pong, *space,
			 *colon, *nullbulk, *nullmultibulk, *queued,
			 *emptymultibulk, *wrongtypeerr, *nokeyerr, *syntaxerr, *sameobjecterr,
			 *outofrangeerr, *loadingerr, *plus,
			 *select[REDIS_SHARED_SELECT_CMDS],
			 *messagebulk, *pmessagebulk, *subscribebulk, *unsubscribebulk, *mbulk3,
			 *mbulk4, *psubscribebulk, *punsubscribebulk,
			 *integers[REDIS_SHARED_INTEGERS];
};

/* Global server state structure */
struct redisServer {
		pthread_t mainthread;
		int arch_bits;
		int port;
		char *bindaddr;
		int ipfd;
		redisDb *db;
		long long dirty;            /* changes to DB from the last save */
		long long dirty_before_bgsave; /* used to restore dirty on failed BGSAVE */
		list *clients;
		dict *commands;             /* Command table hash table */
		/* Fast pointers to often looked up command */
		struct redisCommand *delCommand, *multiCommand;
		//    list *slaves, *monitors;
		redisClient *current_client; /* Current client, only used on crash report */
		char neterr[ANET_ERR_LEN];
		aeEventLoop *el;
		int cronloops;              /* number of times the cron function run */
		/* Fields used only for stats */
		time_t stat_starttime;          /* server start time */
		long long stat_numcommands;     /* number of processed commands */
		long long stat_numconnections;  /* number of connections received */
		size_t stat_peak_memory;        /* max used memory record */
		/* Configuration */
		int verbosity;
		int maxidletime;
		size_t client_max_querybuf_len;
		int dbnum;
		int daemonize;
		int shutdown_asap;
		char *pidfile;
		char *logfile;
		int syslog_enabled;
		char *syslog_ident;
		int syslog_facility;
		/* Limits */
		unsigned int maxclients;
		/* Blocked clients */
		time_t unixtime;    /* Unix time sampled every second. */
		unsigned lruclock:22;        /* clock incrementing every minute, for LRU */
		unsigned lruclock_padding:10;
		/* Assert & bug reportign */
		char *assert_failed;
		char *assert_file;
		int assert_line;
		int bug_report_start; /* True if bug report header already logged. */
		dict *fms;             /* formulas hash table */
};


typedef void redisCommandProc(redisClient *c);
typedef void redisVmPreloadProc(redisClient *c, struct redisCommand *cmd, int argc, robj **argv);


struct redisCommand {
		char *name;
		redisCommandProc *proc;
		int arity;
		int flags;
};

/*-----------------------------------------------------------------------------
 * Extern declarations
 *----------------------------------------------------------------------------*/

extern struct redisServer server;
extern struct sharedObjectsStruct shared;
extern double R_Zero, R_PosInf, R_NegInf, R_Nan;
dictType hashDictType;

/*-----------------------------------------------------------------------------
 * Functions prototypes
 *----------------------------------------------------------------------------*/

/* networking.c -- Networking and Client related operations */
redisClient *createClient(int fd);
void closeTimedoutClients(void);
void freeClient(redisClient *c);
void resetClient(redisClient *c);
void sendReplyToClient(aeEventLoop *el, int fd, void *privdata, int mask);
void addReply(redisClient *c, robj *obj);
void addReplySds(redisClient *c, sds s);
void addReplyBulkCBuffer(redisClient *c, void *p, size_t len);
void addReplyBulkCString(redisClient *c, char *s);
void processInputBuffer(redisClient *c);
void acceptTcpHandler(aeEventLoop *el, int fd, void *privdata, int mask);
void readQueryFromClient(aeEventLoop *el, int fd, void *privdata, int mask);
void addReply(redisClient *c, robj *obj);
void addReplyError(redisClient *c, char *err);
void *dupClientReplyValue(void *o);
void getClientsMaxBuffers(unsigned long *longest_output_list,unsigned long *biggest_input_buffer);
sds getClientInfoString(redisClient *client);
sds getAllClientsInfoString(void);
void addReplyLongLong(redisClient *c, long long ll);

#ifdef __GNUC__
void addReplyErrorFormat(redisClient *c, const char *fmt, ...) __attribute__((format(printf, 2, 3)));
void addReplyStatusFormat(redisClient *c, const char *fmt, ...) __attribute__((format(printf, 2, 3)));
#else
void addReplyErrorFormat(redisClient *c, const char *fmt, ...);
void addReplyStatusFormat(redisClient *c, const char *fmt, ...);
#endif

/* Redis object implementation */
void decrRefCount(void *o);
void incrRefCount(robj *o);
void freeStringObject(robj *o);
robj *createObject(int type, void *ptr);
robj *createStringObject(char *ptr, size_t len);
robj *dupStringObject(robj *o);
robj *getDecodedObject(robj *o);
int getLongFromObjectOrReply(redisClient *c, robj *o, long *target, const char *msg);


/* Core functions */
int processCommand(redisClient *c);
void setupSignalHandlers(void);
struct redisCommand *lookupCommand(sds name);
struct redisCommand *lookupCommandByCString(char *s);
void call(redisClient *c);
int prepareForShutdown();
void redisLog(int level, const char *fmt, ...);
void usage();
void oom(const char *msg);
void populateCommandTable(void);

/* Configuration */
void loadServerConfig(char *filename);
int selectDb(redisClient *c, int id);

void* loadfm(char *f_name);
void setCommand(redisClient *c);
void grunCommand(redisClient *c);
void loadCommand(redisClient *c);
void getCommand(redisClient *c);
void gsh_init();
/*void setexCommand(redisClient *c);
void setnxCommand(redisClient *c);
void delCommand(redisClient *c);
void existsCommand(redisClient *c);
void setbitCommand(redisClient *c);
void getbitCommand(redisClient *c);
void setrangeCommand(redisClient *c);
void getrangeCommand(redisClient *c);
void appendCommand(redisClient *c);
void strlenCommand(redisClient *c);*/
void infoCommand(redisClient *c);

#if defined(__GNUC__)
void *calloc(size_t count, size_t size) __attribute__ ((deprecated));
void free(void *ptr) __attribute__ ((deprecated));
void *malloc(size_t size) __attribute__ ((deprecated));
void *realloc(void *ptr, size_t size) __attribute__ ((deprecated));
#endif

void redisLogObjectDebugInfo(robj *o);

#endif
