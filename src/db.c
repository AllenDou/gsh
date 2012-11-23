#include "gsh.h"
#include <signal.h>


int selectDb(redisClient *c, int id) {
		if (id < 0 || id >= server.dbnum)
				return REDIS_ERR;
		c->db = &server.db[id];
		return REDIS_OK;
}

