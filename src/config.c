#include "gsh.h"

/*-----------------------------------------------------------------------------
 * Config file parsing
 *----------------------------------------------------------------------------*/

int yesnotoi(char *s) {
		if (!strcasecmp(s,"yes")) return 1;
		else if (!strcasecmp(s,"no")) return 0;
		else return -1;
}


/* I agree, this is a very rudimental way to load a configuration...
   will improve later if the config gets more complex */
void loadServerConfig(char *filename) {
		FILE *fp;
		char buf[REDIS_CONFIGLINE_MAX+1], *err = NULL;
		int linenum = 0;
		sds line = NULL;

		if (filename[0] == '-' && filename[1] == '\0')
				fp = stdin;
		else {
				if ((fp = fopen(filename,"r")) == NULL) {
						redisLog(REDIS_WARNING, "Fatal error, can't open config file '%s'", filename);
						exit(1);
				}
		}

		while(fgets(buf,REDIS_CONFIGLINE_MAX+1,fp) != NULL) {
				sds *argv;
				int argc, j;

				linenum++;
				line = sdsnew(buf);
				line = sdstrim(line," \t\r\n");

				/* Skip comments and blank lines*/
				if (line[0] == '#' || line[0] == '\0') {
						sdsfree(line);
						continue;
				}

				/* Split into arguments */
				argv = sdssplitargs(line,&argc);
				sdstolower(argv[0]);

				/* Execute config directives */
				if (!strcasecmp(argv[0],"timeout") && argc == 2) {
						server.maxidletime = atoi(argv[1]);
						if (server.maxidletime < 0) {
								err = "Invalid timeout value"; goto loaderr;
						}
				} else if (!strcasecmp(argv[0],"port") && argc == 2) {
						server.port = atoi(argv[1]);
						if (server.port < 0 || server.port > 65535) {
								err = "Invalid port"; goto loaderr;
						}
				} else if (!strcasecmp(argv[0],"bind") && argc == 2) {
						server.bindaddr = zstrdup(argv[1]);
				} else if (!strcasecmp(argv[0],"formula") && argc == 2) {
						gsh_formula_iask_wordseg_init(0,0);
						//void *val = loadfm(argv[1]);
						//if(!val) goto loaderr;
						//int retval = dictAdd(server.fms, sdsnew(argv[1]), val);
						//if(retval != DICT_OK) goto loaderr;
				} else if (!strcasecmp(argv[0],"dir") && argc == 2) {
						if (chdir(argv[1]) == -1) {
								redisLog(REDIS_WARNING,"Can't chdir to '%s': %s",
												argv[1], strerror(errno));
								exit(1);
						}
				} else if (!strcasecmp(argv[0],"loglevel") && argc == 2) {
						if (!strcasecmp(argv[1],"debug")) server.verbosity = REDIS_DEBUG;
						else if (!strcasecmp(argv[1],"verbose")) server.verbosity = REDIS_VERBOSE;
						else if (!strcasecmp(argv[1],"notice")) server.verbosity = REDIS_NOTICE;
						else if (!strcasecmp(argv[1],"warning")) server.verbosity = REDIS_WARNING;
						else {
								err = "Invalid log level. Must be one of debug, notice, warning";
								goto loaderr;
						}
				} else if (!strcasecmp(argv[0],"logfile") && argc == 2) {
						FILE *logfp;

						server.logfile = zstrdup(argv[1]);
						if (!strcasecmp(server.logfile,"stdout")) {
								zfree(server.logfile);
								server.logfile = NULL;
						}
						if (server.logfile) {
								/* Test if we are able to open the file. The server will not
								 * be able to abort just for this problem later... */
								logfp = fopen(server.logfile,"a");
								if (logfp == NULL) {
										err = sdscatprintf(sdsempty(),
														"Can't open the log file: %s", strerror(errno));
										goto loaderr;
								}
								fclose(logfp);
						}
				} else if (!strcasecmp(argv[0],"databases") && argc == 2) {
						server.dbnum = atoi(argv[1]);
						if (server.dbnum < 1) {
								err = "Invalid number of databases"; goto loaderr;
						}
				} else if (!strcasecmp(argv[0],"maxclients") && argc == 2) {
						server.maxclients = atoi(argv[1]);
				} else if (!strcasecmp(argv[0],"daemonize") && argc == 2) {
						if ((server.daemonize = yesnotoi(argv[1])) == -1) {
								err = "argument must be 'yes' or 'no'"; goto loaderr;
						}
				} else if (!strcasecmp(argv[0],"pidfile") && argc == 2) {
						zfree(server.pidfile);
						server.pidfile = zstrdup(argv[1]);
				} else {
						err = "Bad directive or wrong number of arguments"; goto loaderr;
				}
				for (j = 0; j < argc; j++)
						sdsfree(argv[j]);
				zfree(argv);
				sdsfree(line);
		}
		if (fp != stdin) fclose(fp);
		return;

loaderr:
		fprintf(stderr, "\n*** FATAL CONFIG FILE ERROR ***\n");
		fprintf(stderr, "Reading the configuration file, at line %d\n", linenum);
		fprintf(stderr, ">>> '%s'\n", line);
		fprintf(stderr, "%s\n", err);
		exit(1);

}

