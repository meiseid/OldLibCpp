// 共通
//
#ifndef __libcpp_h__
#define __libcpp_h__

//C
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/rand.h>

//C++
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

//defs

#define strnull(x) (x == NULL || x[0] == '\0')
#define countof(x) sizeof(x) / sizeof(x[0])

#define MAX_SHORT_NAME 32
#define SOCKET_TIMEOUT 10

//Original
#include "cData.h"
#include "cArray.h"
#include "cFile.h"
#include "cSocket.h"
#include "cTool.h"
#include "cApplication.h"

#endif //__libcpp_h__
