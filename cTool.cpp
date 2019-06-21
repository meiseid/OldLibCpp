// 静的ツール
//
#include "libcpp.h"

int cTool::addString( std::string &str,const char *fmt, ... )
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

void cTool::replaceString( std::string &str,const char *from,const char *to )
{
	std::string s_from = std::string(from);
	std::string s_to = std::string(to);

	std::string::size_type pos( str.find( s_from ) );
 
	while( pos != std::string::npos ){
		str.replace( pos,s_from.length(),s_to );
		pos = str.find( s_from,pos + s_to.length() );
	}
}

char* cTool::urlDecode( const char *src,char *dst )
{
	int i,j,num; char buf[8];

	if( dst ) dst[0] = '\0';
	if( !src ) return NULL;
	if( !dst ) dst = (char*)malloc( strlen( src ) + 1 );
	if( !dst ) return NULL;

	for( i = 0,j = 0; src[i] != '\0'; i++ ){
		if( src[i] == '%' ){
			if( src[i + 1] == '\0' || src[i + 2] == '\0' ) break; //overflow
			memcpy( buf,src + i,3 ); buf[3] = '\0';
			sscanf( buf,"%%%02X",&num );
			if( num > 255 ) break; //overflow
			dst[j++] = (char)num; i += 2;
		}else if( src[i] == '+' ) dst[j++] = ' ';
		else dst[j++] = src[i];
	}
	dst[j] = '\0';
	return dst;
}

char* cTool::urlEncode( const char *src,char *dst )
{
	int i,j;

	if( dst ) dst[0] = '\0';
	if( !src ) return NULL;
	if( !dst ) dst = (char*)malloc( strlen( src ) * 3 + 1 );
	if( !dst ) return NULL;

	for( i = 0,j = 0; src[i] != '\0'; i++ ){
		if( (src[i] >= '0' && src[i] <= '9') ||
			(src[i] >= 'A' && src[i] <= 'Z') ||
			(src[i] >= 'a' && src[i] <= 'z') ||
			src[i] == '_' || src[i] == '-' || src[i] == '.' ) dst[j++] = src[i];
		else if( src[i] == ' ' ) dst[j++] = '+';
		else j += sprintf( dst + j,"%%%02X",(unsigned char)(src[i]) );
	}dst[j] = '\0';

	return dst;
}

bool cTool::urlSplit( const char *url,char *host,char *port,char *path )
{
	host[0] = '\0'; port[0] = '\0'; path[0] = '\0';

	int off; char *p1,*p2; size_t len;

	if( strncmp( url,"https://",8 ) == 0 ){
		off = 8;
		strcpy( port,"443" );
	}
	else if( strncmp( url,"http://",7 ) == 0 ){
		off = 7;
		strcpy( port,"80" );
	}
	else{
		return false;
	}

	p1 = (char*)url + off;

	if( (p2 = strchr( p1,'/' )) != NULL ||
		(p2 = strchr( p1,'\0' )) != NULL ){
		len = (size_t)p2 - (size_t)p1;
		memcpy( host,p1,len );
		host[len] = '\0';
		if( *p1 == '\0' ) strcpy( path,"/" );
		else strcpy( path,p2 );
	}

	return (host[0] != '\0' ? true:false);
}

bool cTool::readConfigFile( const char *path,cArray &array )
{
	char *mem,*p1,*p2,*p3,*p4,key[MAX_SHORT_NAME],*val; int i,n; size_t len;

	cFile file = cFile( path );
	file.read();

	if( !file.DATA ) return false; //read fail
	mem = (char*)file.DATA;

	for( p1 = mem,n = 0; (p2 = strstr( p1,"=\"" )) != NULL; p1 = p2 + 2 ){ n++; }
	if( n <= 0 ) return false; //NO CONFIG

	array.init( n );

	for( i = 0,p1 = mem; (p2 = strstr( p1,"=\"" )) != NULL; p1 = p2 + 2 ){
		if( p1 == p2 || *(p2 + 2) == '\0' || *(p2 + 2) == '"' ) continue;
		for( p3 = p2 - 1; p3 != mem && *p3 != '\r' && *p3 != '\n'; p3-- ){ ; }
		if( p3 != mem ) p3++; if( *p3 == '#' ) continue;
		if( (p4 = strchr( p2 + 2,'"' )) == NULL ) p4 = strchr( p2 + 2,'\0' );
		len = (size_t)p2 - (size_t)p3;
		if( len <= 0 || len >= MAX_SHORT_NAME ) continue; //カラの内容 または 長い項目名はダメ
		memcpy( key,p3,len ); key[len] = '\0';
		p2 += 2; len = (size_t)p4 - (size_t)p2;
		if( len > 0 && (val = (char*)malloc( len + 1 )) != NULL ){
			memcpy( val,p2,len ); val[len] = '\0';
			cData *data = array.indexAt(i++);
			strcpy( data->SNAME,key );
			data->TEXT = val;
			data->TEXTLEN = len;
		}
	}

	return true; //complete
}

bool cTool::readSinglePart( char *mem,size_t len,char sep,cArray &array )
{
	char *fr,*to,*bg,*en,key[MAX_SHORT_NAME],*val; size_t k_len,v_len; int i,ok;

	for( fr = mem,ok = 0; (to = strchr( fr,'=' )) != NULL; fr = to + 1 ){
		if( fr != to && *(to + 1) != '\0' ) ok++;
	}if( ok <= 0 ) return false; //NOP

	array.init( ok );

	for( i = 0,fr = mem; (to = strchr( fr,'=' )) != NULL; fr = to + 1 ){
		val = NULL; ok = 0;
		k_len = (size_t)to - (size_t)fr; bg = to + 1;
		if( (en = strchr( bg,sep )) != NULL ){ *(to = en) = '\0'; }
		else{ en = mem + len; to = en - 1; }
		if( bg[0] != '\0' && //カラの内容 または 長い項目名はダメ
			k_len > 0 && k_len < MAX_SHORT_NAME ){ 
			memcpy( key,fr,k_len ); key[k_len] = '\0';
			if( array.snameAt( key ) == NULL ){ //未登録の項目
				if( (val = urlDecode( bg,NULL )) != NULL ){
					v_len = strlen( val );
					cData *data = array.indexAt(i++);
					strcpy( data->SNAME,key );
					data->TEXT = val;
					data->TEXTLEN = v_len;
					ok = 1;
				}
			}
		}
		if( !ok ){ if( val ) free( val ); } //read error
		if( *(to + 1) == ' ' ) to++;
	}

	return true; //complete
}
