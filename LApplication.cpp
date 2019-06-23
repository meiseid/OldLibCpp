// アプリケーション
//
#include "libcpp.h"

LApplication::LApplication( int argc,char **argv )
{
	mLogStr.append( LTool::clockText( "BEGINNING OF CGI" ) );

	char *p;

	readCgiParams();

	time_t tm = time( NULL );
	mDate = *(localtime( &tm ));
	mDate.tm_sec = 0; //秒は常に0
	mktime( &mDate );
	mDateStr = NULL;

	if( (p = LTool::paramText( "dgdate",mCgi )) != NULL && strlen( p ) == 12 ){
		mDateStr = p; //YYYYMMDDHHMM
		sscanf( p,"%04d%02d%02d%02d%02d",&mDate.tm_year,&mDate.tm_mon,
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
	mLogStr.append( LTool::clockText( "ENTER PRINT" ) );

	if( !mOutHead.empty() ){
		std::cout << mOutHead;
		mLogStr.append( mOutHead );
	}
	if( !mOutBody.empty() ){
		std::cout << mOutBody;
		mLogStr.append( mOutBody );
	}

	mLogStr.append( LTool::clockText( "LEAVE PRINT" ) );

	mLogStr.append( LTool::clockText( "END OF CGI" ) );

	if( !mLogPath.empty() && !mLogStr.empty() ){
		std::ofstream logfile( mLogPath );
		logfile << mLogStr;
		logfile.close();
	}
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

	mLogStr.append( mem,len );
	mLogStr.append( "\n" );

	LTool::readSinglePart( mem,len,'&',mCgi );

}
