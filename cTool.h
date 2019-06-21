// 静的ツール
//
#ifndef __cTool_h__
#define __cTool_h__
#include "libcpp.h"

class cTool
{
public:
	static int addString( std::string &str,const char *fmt, ... );
	static void replaceString( std::string &str,const char *from,const char *to );
	static char* urlDecode( const char *src,char *dst );
	static char* urlEncode( const char *src,char *dst );
	static bool urlSplit( const char *url,char *host,char *port,char *path );
	static bool readConfigFile( const char *path,cArray &array );
	static bool readSinglePart( char *mem,size_t len,char sep,cArray &array );

};

#endif //__cTool_h__
