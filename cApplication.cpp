// アプリケーション
//
#include "libcpp.h"

cApplication::cApplication()
{
	CONFIG = NULL;
	CGI = NULL;
}

cApplication::~cApplication( void )
{
	if( CONFIG ) delete CONFIG;
	if( CGI ) delete CGI;
}

void cApplication::run( int argc,char **argv )
{
	loadCgiParams();
}

void cApplication::loadConfigFile( const char *path )
{
	char *mem,*p1,*p2,*p3,*p4,key[MAX_SHORT_NAME],*val; int i,n; size_t len;

	cFile file = cFile(path);
	file.read();

	if( !file.DATA ) return;
	mem = (char*)file.DATA;

	for( p1 = mem,n = 0; (p2 = strstr( p1,"=\"" )) != NULL; p1 = p2 + 2 ){ n++; }
	if( n <= 0 ) return; //NO CONFIG

	CONFIG = new cArray( n );

	for( i = 0,p1 = mem; (p2 = strstr( p1,"=\"" )) != NULL; p1 = p2 + 2 ){
		if( p1 == p2 || *(p2 + 2) == '\0' || *(p2 + 2) == '"' ) continue;
		for( p3 = p2 - 1; p3 != mem && *p3 != '\r' && *p3 != '\n'; p3-- ){ ; }
		if( p3 != mem ) p3++; if( *p3 == '#' ) continue;
		if( (p4 = strchr( p2 + 2,'"' )) == NULL ) p4 = strchr( p2 + 2,'\0' );
		len = (size_t)p2 - (size_t)p3;
		if( len <= 0 || len >= MAX_SHORT_NAME ) continue; //長い項目名はダメ
		memcpy( key,p3,len ); key[len] = '\0';
		p2 += 2; len = (size_t)p4 - (size_t)p2;
		if( (val = (char*)malloc( len + 1 )) != NULL ){
			memcpy( val,p2,len ); val[len] = '\0';
			cData *data = CONFIG->indexAt(i++);
			strcpy( data->SNAME,key );
			data->TEXT = val;
			data->TEXTLEN = len;
		}
	}
}

void cApplication::loadCgiParams( void )
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

	CGI = readSinglePart( mem,len,'&' );
}

cArray* cApplication::readSinglePart( char *mem,size_t len,char sep )
{
	char *fr,*to,*bg,*en,key[MAX_SHORT_NAME],*val; size_t k_len,v_len; int i,ok;
	cArray *ret = NULL;

	for( fr = mem,ok = 0; (to = strchr( fr,'=' )) != NULL; fr = to + 1 ){
		if( fr != to && *(to + 1) != '\0' ) ok++;
	}if( ok <= 0 ) return NULL; //NOP

	ret = new cArray( ok );

	for( i = 0,fr = mem; (to = strchr( fr,'=' )) != NULL; fr = to + 1 ){
		val = NULL; ok = 0;
		k_len = (size_t)to - (size_t)fr; bg = to + 1;
		if( (en = strchr( bg,sep )) != NULL ){ *(to = en) = '\0'; }
		else{ en = mem + len; to = en - 1; }
		if( k_len > 0 && k_len < MAX_SHORT_NAME ){ //長い項目名はダメ
			memcpy( key,fr,k_len ); key[k_len] = '\0';
			if( ret->snameAt( key ) == NULL ){ //未登録の項目
				if( (val = urlDecode( bg,NULL )) != NULL ){
					v_len = strlen( val );
					cData *data = ret->indexAt(i++);
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
	return ret; //allocated array
}

char* cApplication::urlDecode( const char *src,char *dst )
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

char* cApplication::urlEncode( const char *src,char *dst )
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

