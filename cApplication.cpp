// アプリケーション
//
#include "libcpp.h"

cApplication::cApplication( void )
{
	readCgiParams();

	time_t tm = time( NULL );
	DATE = *(localtime( &tm ));
	DATE.tm_sec = 0; //秒は常に0
	mktime( &DATE );
	DEBUG_DATE = NULL;

	cData* data;
	if( (data = CGI.snameAt("DEBUG_DATE")) != NULL &&
		strlen( data->TEXT ) == 12 ){ //YYYYMMDDHHMM
		DEBUG_DATE = data->TEXT;
		sscanf( data->TEXT,"%04d%02d%02d%02d%02d",
			&DATE.tm_year,&DATE.tm_mon,&DATE.tm_mday,&DATE.tm_hour,&DATE.tm_min );
		DATE.tm_year -= 1900; 
		DATE.tm_mon--;
		mktime( &DATE );
	}
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
