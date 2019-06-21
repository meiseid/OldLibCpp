// アプリケーション
//
#include "libcpp.h"

cApplication::cApplication( void )
{
	readCgiParams();
}

cApplication::~cApplication( void )
{

}

void cApplication::run( int argc,char **argv )
{

}

void cApplication::readCgiParams( void )
{
	char *ptr,*mem = NULL; size_t len = 0;

	if( (ptr = getenv( "QUERY_STRING" )) != NULL && (len = strlen( ptr )) > 0 ){
		mem = strdup( ptr );
	}else if( (ptr = getenv( "CONTENT_LENGTH" )) != NULL && (len = atoi( ptr )) > 0 ){
		if( (mem = (char*)malloc( len + 1 )) != NULL ){
			fread( mem,len,1,stdin ); mem[len] = '\0';
		}
	}
	if( !mem ) return; //NOT CGI

	cTool::readSinglePart( mem,len,'&',CGI );
}
