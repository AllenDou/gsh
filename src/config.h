#ifndef __CONFIG_H
#define __CONFIG_H

/* Test for proc filesystem */
#ifdef __linux__
#define HAVE_PROCFS 1
#endif

/* Test for backtrace() */
#if defined(__APPLE__) || defined(__linux__) || defined(__sun)
#define HAVE_BACKTRACE 1
#endif

/* Test for polling API */
#ifdef __linux__
#define HAVE_EPOLL 1
#endif

#if (defined(__APPLE__) && defined(MAC_OS_X_VERSION_10_6)) || defined(__FreeBSD__) || defined(__OpenBSD__) || defined (__NetBSD__)
#define HAVE_KQUEUE 1
#endif

/* Define aof_fsync to fdatasync() in Linux and fsync() for all the rest */
#ifdef __linux__
#define aof_fsync fdatasync
#else
#define aof_fsync fsync
#endif

#endif
