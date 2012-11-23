#include "common/fmacros.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>

#include "anet.h"

static void anetSetError(char *err, const char *fmt, ...)
{
		va_list ap;

		if (!err) return;
		va_start(ap, fmt);
		vsnprintf(err, ANET_ERR_LEN, fmt, ap);
		va_end(ap);
}

int anetNonBlock(char *err, int fd)
{
		int flags;

		/* Set the socket nonblocking.
		 * Note that fcntl(2) for F_GETFL and F_SETFL can't be
		 * interrupted by a signal. */
		if ((flags = fcntl(fd, F_GETFL)) == -1) {
				anetSetError(err, "fcntl(F_GETFL): %s", strerror(errno));
				return ANET_ERR;
		}
		if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
				anetSetError(err, "fcntl(F_SETFL,O_NONBLOCK): %s", strerror(errno));
				return ANET_ERR;
		}
		return ANET_OK;
}

int anetTcpNoDelay(char *err, int fd)
{
		int yes = 1;
		if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes)) == -1)
		{
				anetSetError(err, "setsockopt TCP_NODELAY: %s", strerror(errno));
				return ANET_ERR;
		}
		return ANET_OK;
}


static int anetCreateSocket(char *err, int domain) {
		int s, on = 1;
		if ((s = socket(domain, SOCK_STREAM, 0)) == -1) {
				anetSetError(err, "creating socket: %s", strerror(errno));
				return ANET_ERR;
		}

		/* Make sure connection-intensive things like the gsh benckmark
		 * will be able to close/open sockets a zillion of times */
		if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1) {
				anetSetError(err, "setsockopt SO_REUSEADDR: %s", strerror(errno));
				return ANET_ERR;
		}
		return s;
}

static int anetListen(char *err, int s, struct sockaddr *sa, socklen_t len) {
		if (bind(s,sa,len) == -1) {
				anetSetError(err, "bind: %s", strerror(errno));
				close(s);
				return ANET_ERR;
		}
		if (listen(s, 511) == -1) { /* the magic 511 constant is from nginx */
				anetSetError(err, "listen: %s", strerror(errno));
				close(s);
				return ANET_ERR;
		}
		return ANET_OK;
}

int anetTcpServer(char *err, int port, char *bindaddr)
{
		int s;
		struct sockaddr_in sa;

		if ((s = anetCreateSocket(err,AF_INET)) == ANET_ERR)
				return ANET_ERR;

		memset(&sa,0,sizeof(sa));
		sa.sin_family = AF_INET;
		sa.sin_port = htons(port);
		sa.sin_addr.s_addr = htonl(INADDR_ANY);
		if (bindaddr && inet_aton(bindaddr, &sa.sin_addr) == 0) {
				anetSetError(err, "invalid bind address");
				close(s);
				return ANET_ERR;
		}
		if (anetListen(err,s,(struct sockaddr*)&sa,sizeof(sa)) == ANET_ERR)
				return ANET_ERR;
		return s;
}

static int anetGenericAccept(char *err, int s, struct sockaddr *sa, socklen_t *len) {
		int fd;
		while(1) {
				fd = accept(s,sa,len);
				if (fd == -1) {
						if (errno == EINTR)
								continue;
						else {
								anetSetError(err, "accept: %s", strerror(errno));
								return ANET_ERR;
						}
				}
				break;
		}
		return fd;
}

int anetTcpAccept(char *err, int s, char *ip, int *port) {
		int fd;
		struct sockaddr_in sa;
		socklen_t salen = sizeof(sa);
		if ((fd = anetGenericAccept(err,s,(struct sockaddr*)&sa,&salen)) == ANET_ERR)
				return ANET_ERR;

		if (ip) strcpy(ip,inet_ntoa(sa.sin_addr));
		if (port) *port = ntohs(sa.sin_port);
		return fd;
}

int anetPeerToString(int fd, char *ip, int *port) {
		struct sockaddr_in sa;
		socklen_t salen = sizeof(sa);

		if (getpeername(fd,(struct sockaddr*)&sa,&salen) == -1) return -1;
		if (ip) strcpy(ip,inet_ntoa(sa.sin_addr));
		if (port) *port = ntohs(sa.sin_port);
		return 0;
}
