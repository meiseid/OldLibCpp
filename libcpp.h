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
#include <sstream>
#include <string>
#include <vector>
#include <array>

//defines
#define strnull(x) (x == NULL || x[0] == '\0')
#define countof(x) sizeof(x) / sizeof(x[0])

//structs
typedef struct {
	std::string mName;
	std::string mText;
}SParam;

//classes
#include "LFile.h"
#include "LSocket.h"
#include "LTool.h"
#include "LApplication.h"

#endif //__libcpp_h__
