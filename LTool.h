// 静的ツール
//
#ifndef __LTool_h__
#define __LTool_h__
#include "libcpp.h"

class LTool
{
public:
	static int sizeFile( const char *path,time_t *mtime );
	static void urlDecode( std::string &src,std::string &dst );
	static void urlEncode( std::string &src,std::string &dst );
	static void xmlDecode( std::string &src,std::string &dst );
	static void xmlEncode( std::string &src,std::string &dst );
	static std::string readTextFile( const char *path );
	static void readConfigFile( std::string path,std::vector<SParam> &params );
	static void readSinglePart( char *mem,size_t len,char sep,std::vector<SParam> &params );
	static char* paramText( const char *key,const std::vector<SParam> &params );
	static std::string clockText( std::string label );
	static std::string trimingText( const std::string &src );
	static void getDateText( const struct tm *src,char *dst,size_t len );
	static int strAppend( std::string &str,const char *fmt, ... );
	static void strReplace( std::string &str,std::string from,std::string to );
	static int strSplit( const char *src,char key,std::vector<std::string> &dst );
	static void makeCostString( int cost_num,char *cost_str );
	static void headerText( const char *src,char *dst,int count );
	static char* hankakuToZenkaku( const char *src,char *dst );
	static char* zenkakuToHankaku( const char *src,char *dst );
	static unsigned char* base64Encode( const unsigned char *str,int length,int *ret_length );
};

#endif //__LTool_h__
