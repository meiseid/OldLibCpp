// アプリケーション
//
#include "libcpp.h"

LApplication::LApplication( int argc,char **argv )
{
	char *ptr;

	readCgiParams();

	time_t tm = time( NULL );
	mDate = *(localtime( &tm ));
	mDate.tm_sec = 0; //秒は常に0
	mktime( &mDate );
	mDateStr = NULL;

	if( (ptr = (char*)LTool::paramText( "debug_date",mCgi )) != NULL &&
		strlen( ptr ) == 12 ){ //YYYYMMDDHHMM
		mDateStr = ptr;
		sscanf( ptr,"%04d%02d%02d%02d%02d",&mDate.tm_year,&mDate.tm_mon,
			&mDate.tm_mday,&mDate.tm_hour,&mDate.tm_min );
		mDate.tm_year -= 1900; 
		mDate.tm_mon--;
		mktime( &mDate );
	}
}

LApplication::~LApplication( void )
{

}

void LApplication::run( void )
{

}

void LApplication::finish( void )
{



}

void LApplication::readCgiParams( void )
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

	LTool::readSinglePart( mem,len,'&',mCgi );
}
