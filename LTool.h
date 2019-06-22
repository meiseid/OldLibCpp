// 静的ツール
//
#ifndef __LTool_h__
#define __LTool_h__
#include "libcpp.h"

class LTool
{
public:
	static std::string clockText( std::string label );
	static int strAppend( std::string &str,const char *fmt, ... );
	static void strReplace( std::string &str,std::string from,std::string to );
	static void urlDecode( std::string &src,std::string &dst );
	static void urlEncode( std::string &src,std::string &dst );
	static void readConfigFile( std::string path,std::vector<SParam> &params );
	static void readSinglePart( char *mem,size_t len,char sep,std::vector<SParam> &params );
	static const char* paramText( const char *key,const std::vector<SParam> &params );
};

#endif //__LTool_h__
