#ifndef EASYGO_NETWORK_NETTYPE_H
#define EASYGO_NETWORK_NETTYPE_H

#define LISTEN_BACKLOG	2048

#ifdef WIN32
#define	 FD_SETSIZE	  LISTEN_BACKLOG // must define FD_SETSIZE before including Winsock2.h
#include <WinSock2.h>
#include <WinBase.h>
#include <Windows.h>

#define TNetScoket	SOCKET
typedef short				int16_t;
typedef int					int32_t;
typedef	long long			int64_t;
typedef unsigned char		uint8_t;
typedef unsigned short		uint16_t;
typedef unsigned int		uint32_t;
typedef	unsigned long long	uint64_t;
typedef int					socklen_t;

///////////////////////////
// else WIN32
#else 

#include <signal.h>
#include <syslog.h>
#include<sys/types.h> 
#include<sys/stat.h>
#include <sys/resource.h>

typedef int	SOCKET;
typedef int BOOL;
const int SOCKET_ERROR = -1;
const int INVALID_SOCKET = -1;

#if defined(__APPLE__) || defined(__FreeBSD__)
#include <sys/event.h>
#include <sys/time.h>
#include <sys/syscall.h>

const int TRUE = 1;
const int FALSE = 0;

#else
#include <sys/epoll.h>
#endif

#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>

#define closesocket close
#define ioctlsocket ioctl
#define TNetScoket	int

///////////////////////////
// endif WIN32
#endif

#endif // EASYGO_NETWORK_NETTYPE_H