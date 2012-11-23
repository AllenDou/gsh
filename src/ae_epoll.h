#include <sys/epoll.h>
#include <unistd.h>

#include "ae.h"
#include "common/zmalloc.h"

typedef struct aeApiState {
		int epfd;
		struct epoll_event events[AE_SETSIZE];
} aeApiState;

int aeApiCreate(aeEventLoop *eventLoop);
void aeApiFree(aeEventLoop *eventLoop) ;
int aeApiAddEvent(aeEventLoop *eventLoop, int fd, int mask);
void aeApiDelEvent(aeEventLoop *eventLoop, int fd, int delmask);
int aeApiPoll(aeEventLoop *eventLoop, struct timeval *tvp);
char *aeApiName(void);
