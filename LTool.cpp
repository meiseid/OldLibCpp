// 静的ツール
//
#include "libcpp.h"

int LTool::sizeFile( const char *path,time_t *mtime )
{
	struct stat status;

	if( mtime ) *mtime = 0;

	if( strnull(path) ) return (-1);

	if( stat( path, &status ) == 0 ){
		if( mtime ) *mtime = status.st_mtime;
		return status.st_size; //file size
	}
	return (-1); //not exists
}

void LTool::urlDecode( std::string &src,std::string &dst )
{
	if( src.empty() ) return; //NOP
	int i,num; char buf[8];
	const char *c_src = src.c_str();

	for( i = 0; c_src[i] != '\0'; i++ ){
		if( c_src[i] == '%' ){
			if( c_src[i + 1] == '\0' || c_src[i + 2] == '\0' ){
				break; //overflow
			}
			memcpy( buf,c_src + i,3 ); buf[3] = '\0';
			sscanf( buf,"%%%02X",&num );
			if( num > 255 ){
				break; //overflow
			}
			dst.append( 1,(char)num );
			i += 2;
		}
		else if( c_src[i] == '+' ){
			dst.append( 1,' ' );
		}
		else{
			dst.append( 1,c_src[i] );
		}
	}
}

void LTool::urlEncode( std::string &src,std::string &dst )
{
	if( src.empty() ) return; //NOP
	int i; char buf[8];
	const char *c_src = src.c_str();

	for( i = 0; c_src[i] != '\0'; i++ ){
		if( (c_src[i] >= '0' && c_src[i] <= '9') ||
			(c_src[i] >= 'A' && c_src[i] <= 'Z') ||
			(c_src[i] >= 'a' && c_src[i] <= 'z') ||
			c_src[i] == '_' || c_src[i] == '-' || c_src[i] == '.' ){
			dst.append( 1,c_src[i] );
		}
		else if( c_src[i] == ' ' ){
			dst.append( 1,'+' );
		}
		else{
			sprintf( buf,"%%%02X",(unsigned char)(c_src[i]) );
			dst.append( buf,3 );
		}
	}
}

std::string LTool::readTextFile( const char *path )
{
	std::string reply = std::string();
	LFile file = LFile();
	file.mPath.append( path );
	file.readTextFile();
	if( !file.mText.empty() ){
		reply.append( file.mText );
	}
	return reply;
}

void LTool::readConfigFile( std::string path,std::vector<SParam> &params )
{
	char *mem,*p1,*p2,*p3,*p4; int i,n; size_t len; SParam param;

	LFile file = LFile();
	file.mPath = path;
	file.readTextFile();
	if( file.mText.empty() ) return; //read fail
	mem = (char*)file.mText.c_str();

	for( i = 0,p1 = mem; (p2 = strstr( p1,"=\"" )) != NULL; p1 = p2 + 2 ){
		if( p1 == p2 || *(p2 + 2) == '\0' || *(p2 + 2) == '"' ) continue;
		for( p3 = p2 - 1; p3 != mem && *p3 != '\r' && *p3 != '\n'; p3-- ){ ; }
		if( p3 != mem ) p3++; if( *p3 == '#' ) continue;
		if( (p4 = strchr( p2 + 2,'"' )) == NULL ) p4 = strchr( p2 + 2,'\0' );
		len = (size_t)p2 - (size_t)p3;
		if( len <= 0 ) continue;
		param.mName.clear();
		param.mName.append( p3,len );
		p2 += 2; len = (size_t)p4 - (size_t)p2;
		if( len <= 0 ) continue;
		param.mText.clear();
		param.mText.append( p2,len );
		params.push_back( param );
	}
}

void LTool::readSinglePart( char *mem,size_t len,char sep,std::vector<SParam> &params )
{
	char *fr,*to,*bg,*en; size_t k_len; int i; SParam param;
	std::string ts = std::string();

	for( i = 0,fr = mem; (to = strchr( fr,'=' )) != NULL; fr = to + 1 ){
		k_len = (size_t)to - (size_t)fr;
		bg = to + 1;
		if( (en = strchr( bg,sep )) != NULL ){ *(to = en) = '\0'; }
		else{ en = mem + len; to = en - 1; }
		if( bg[0] != '\0' && k_len > 0 ){ 
			param.mName.clear();
			param.mName.append( fr,k_len );
			ts.clear();
			ts.append( bg );
			param.mText.clear();
			urlDecode( ts,param.mText );
			params.push_back( param );
		}
		if( *(to + 1) == ' ' ) to++;
	}
}

char* LTool::paramText( const char *key,const std::vector<SParam> &params )
{
	for( int i = 0,n = params.size(); i < n; i++ ){
		if( strcasecmp( params.at( i ).mName.c_str(),key ) == 0 ){
			return (char*)params.at( i ).mText.c_str();
		}
	}
	return NULL;
}

std::string LTool::clockText( std::string label )
{
	time_t tm; struct tm date; struct timeval tv; int msec; char tstr[128];

	tm = time( NULL );
	date = *(localtime( &tm ));
	gettimeofday( &tv,NULL );
	msec = (int)(tv.tv_usec / 1000);

	sprintf( tstr,"%04d-%02d-%02d %02d:%02d:%02d.%03d ",
		date.tm_year + 1900,date.tm_mon + 1,
		date.tm_mday,date.tm_hour,date.tm_min,date.tm_sec,msec );

	std::string ret( tstr );
	ret.append( label );
	ret.append( "\n" );
	return ret;
}

int LTool::strAppend( std::string &str,const char *fmt, ... )
{
	int ret = 0; va_list va; char *ptr = NULL;

	va_start( va,fmt );
	ret = vasprintf( &ptr,fmt,va );
	va_end( va );

	if( ret >= 0 && ptr ){
		str.append( ptr );
		free( ptr );
	}else ret = 0;

	return ret;
}

void LTool::strReplace( std::string &str,std::string from,std::string to )
{
	std::string::size_type pos( str.find( from ) );

	while( pos != std::string::npos ){
		str.replace( pos,from.length(),to );
		pos = str.find( from,pos + to.length() );
	}
}

