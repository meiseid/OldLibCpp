// アプリケーション
//
#ifndef __cApplication_h__
#define __cApplication_h__
#include "libcpp.h"

class cApplication
{
public:
	cArray *CONFIG;
	cArray *CGI;

	cApplication();
	virtual	~cApplication( void );
	virtual void run( int argc,char **argv );
	void loadConfigFile( const char *path );
	void loadCgiParams( void );
	cArray* readSinglePart( char *mem,size_t len,char sep );
	char* urlDecode( const char *src,char *dst );
	char* urlEncode( const char *src,char *dst );
};

#endif //__cApplication_h__
