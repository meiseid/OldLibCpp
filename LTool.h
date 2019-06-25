// 静的ツール
//
#ifndef __LTool_h__
#define __LTool_h__
#include "libcpp.h"

class LTool
{
public:
	static void urlDecode( std::string &src,std::string &dst );
	static void urlEncode( std::string &src,std::string &dst );
	static std::string readTextFile( const char *path );
	static void readConfigFile( std::string path,std::vector<SParam> &params );
	static void readSinglePart( char *mem,size_t len,char sep,std::vector<SParam> &params );
	static char* paramText( const char *key,const std::vector<SParam> &params );
	static std::string clockText( std::string label );
	static int strAppend( std::string &str,const char *fmt, ... );
	static void strReplace( std::string &str,std::string from,std::string to );
};

#endif //__LTool_h__
