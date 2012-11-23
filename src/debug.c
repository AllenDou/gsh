#include "gsh.h"
#include <arpa/inet.h>

void redisLogObjectDebugInfo(robj *o) {
		redisLog(REDIS_WARNING,"Object type: %d", o->type);
		redisLog(REDIS_WARNING,"Object encoding: %d", o->encoding);
		redisLog(REDIS_WARNING,"Object refcount: %d", o->refcount);
		if (o->type == REDIS_STRING && o->encoding == REDIS_ENCODING_RAW) {
				redisLog(REDIS_WARNING,"Object raw string len: %d", sdslen(o->ptr));
				if (sdslen(o->ptr) < 4096)
						redisLog(REDIS_WARNING,"Object raw string content: \"%s\"", (char*)o->ptr);
		}
}

void _redisAssert(char *estr, char *file, int line) {
#ifdef HAVE_BACKTRACE
		bugReportStart();
#endif
		redisLog(REDIS_WARNING,"=== ASSERTION FAILED ===");
		redisLog(REDIS_WARNING,"==> %s:%d '%s' is not true",file,line,estr);
#ifdef HAVE_BACKTRACE
		server.assert_failed = estr;
		server.assert_file = file;
		server.assert_line = line;
		redisLog(REDIS_WARNING,"(forcing SIGSEGV to print the bug report.)");
#endif
		*((char*)-1) = 'x';
}

void _redisPanic(char *msg, char *file, int line) {
#ifdef HAVE_BACKTRACE
		bugReportStart();
#endif
		redisLog(REDIS_WARNING,"!!! Software Failure. Press left mouse button to continue");
		redisLog(REDIS_WARNING,"Guru Meditation: %s #%s:%d",msg,file,line);
#ifdef HAVE_BACKTRACE
		redisLog(REDIS_WARNING,"(forcing SIGSEGV in order to print the stack trace)");
#endif
		*((char*)-1) = 'x';
}
