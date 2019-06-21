// アプリケーション
//
#ifndef __cApplication_h__
#define __cApplication_h__
#include "libcpp.h"

class cApplication
{
public:
	std::string LOG;
	std::string ERR;

	cArray CONFIG;
	cArray CGI;
	struct tm DATE;
	char *DEBUG_DATE;

	cApplication( int argc,char **argv );
	cApplication( void );
	virtual	~cApplication( void );
	virtual void run( void );
	virtual void finish( void );
	void readCgiParams( void );
};

#endif //__cApplication_h__
