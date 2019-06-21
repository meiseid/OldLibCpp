// アプリケーション
//
#ifndef __cApplication_h__
#define __cApplication_h__
#include "libcpp.h"

class cApplication
{
public:
	cArray CONFIG;
	cArray CGI;

	cApplication( void );
	virtual	~cApplication( void );
	virtual void run( int argc,char **argv );
	void readCgiParams( void );
};

#endif //__cApplication_h__
