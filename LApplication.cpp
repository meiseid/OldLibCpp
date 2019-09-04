// アプリケーション
//
#include "libcpp.h"

LApplication::LApplication( int argc,char **argv )
{
	mLogStr.append( LTool::clockText( "BEGINNING OF CGI" ) );

	char *p;

	readCgiParams();
	readCookieParams();

	time_t tm = time( NULL );
	mDate = *(localtime( &tm ));
	mDate.tm_sec = 0; //秒は常に0
	mTime = mktime( &mDate );
	mDateStr = NULL;

	if( (p = LTool::paramText( "dgdate",mCgi )) != NULL && strlen( p ) == 12 ){
		mDateStr = p; //YYYYMMDDHHMM
		sscanf( p,"%04d%02d%02d%02d%02d",&mDate.tm_year,&mDate.tm_mon,
			&mDate.tm_mday,&mDate.tm_hour,&mDate.tm_min );
		mDate.tm_year -= 1900; 
		mDate.tm_mon--;
		mTime = mktime( &mDate );
	}

	if( (p = getenv( "HTTP_X_FORWARDED_FOR" )) != NULL ||
		(p = getenv( "REMOTE_ADDR" )) != NULL ){
		mClientIp.append( p );
	}
	else{
		mClientIp.append( "127.0.0.1" );
	}
	//mClientIp = std::string( "182.171.232.49, 64.252.172.75" );

	std::string::size_type n = mClientIp.find(',');
	if( n != std::string::npos ){
		mClientIp = mClientIp.substr( 0,n );
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

void LApplication::readCookieParams( void )
{
	char *ptr,*mem = NULL; size_t len = 0; int i,j;

	if( (ptr = getenv( "HTTP_COOKIE" )) != NULL && (len = strlen( ptr )) > 0 &&
		(mem = (char*)malloc( len + 32 )) != NULL ){
		for( i = 0,j = 0; ptr[i] != '\0'; i++ ){
			if( ptr[i] == ';' && ptr[i + 1] == ' ' ){
				mem[j++] = ptr[i]; i++;
			}
			else{
				mem[j++] = ptr[i];
			}
		}
		mem[j] = '\0'; len = j;
		mLogStr.append( mem,len);
		mLogStr.append( "\n" );
		LTool::readSinglePart( mem,len,';',mCookie );
	}
}
