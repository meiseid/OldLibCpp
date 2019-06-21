// 静的ツール
//
#ifndef __cTool_h__
#define __cTool_h__
#include "libcpp.h"

class cTool
{
public:

	static char* urlDecode( const char *src,char *dst );
	static char* urlEncode( const char *src,char *dst );
	static bool urlSplit( const char *url,char *host,char *port,char *path );
	static bool readConfigFile( const char *path,cArray &array );
	static bool readSinglePart( char *mem,size_t len,char sep,cArray &array );

};

#endif //__cTool_h__
