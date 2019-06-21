// ソケット通信
//
#include "libcpp.h"

cSocket::cSocket( void )
{
	HOST = NULL;
	PORT = NULL;
	HANDLE = (-1);
	H_SSL_CTX = NULL;
	H_SSL = NULL;
	CFILE = NULL;
	HEAD = NULL;
	HLEN = 0;
	BODY = NULL;
	BLEN = 0;
}

cSocket::~cSocket( void )
{
	if( HEAD ) free( HEAD );
	if( BODY ) free( BODY );
	if( CFILE ) delete CFILE;
	disconnect();
}

bool cSocket::connect( void )
{
	if( !HOST || !PORT ) return false;

	disconnect();

	struct sockaddr_in addr; struct hostent *servhost; int ret;
	unsigned short rand_ret;

	servhost = gethostbyname( HOST );
	if( (HANDLE = socket( AF_INET,SOCK_STREAM,0 )) >= 0 ){
		addr.sin_family = AF_INET;
		if( servhost ) memcpy( (char*)&addr.sin_addr,servhost->h_addr,servhost->h_length );
		else addr.sin_addr.s_addr = inet_addr( HOST );	
		addr.sin_port = htons( atoi( PORT ) );
		setTimeout( SOCKET_TIMEOUT );
		ret = ::connect( HANDLE,(struct sockaddr *)&addr,sizeof(addr) );
		if( ret >= 0 ){
			if( atoi( PORT ) == 443 ){
				SSL_load_error_strings();
				SSL_library_init();
				if( (H_SSL_CTX = SSL_CTX_new( SSLv23_client_method() )) != NULL ){
					if( (H_SSL = SSL_new( H_SSL_CTX )) != NULL ){
						if( (ret = SSL_set_fd( H_SSL,HANDLE )) != 0 ){
							RAND_poll();
							while( RAND_status() == 0 ){
								rand_ret = rand() % 65536;
								RAND_seed( &rand_ret,sizeof(rand_ret) );
							}
							if( (ret = SSL_connect( H_SSL )) == 1 ){
								return true; //complete
							}
						}
					}
				}
			}
			else{
				return true; //complete
			}
		}
		disconnect();
	}

	return false; //FAIL
}

void cSocket::disconnect( void )
{
	if( H_SSL ){
		SSL_shutdown( H_SSL );
		SSL_free( H_SSL );
		H_SSL = NULL;
	}

	if( H_SSL_CTX ){
		SSL_CTX_free( H_SSL_CTX );
		H_SSL_CTX = NULL;
	}

	if( HANDLE >= 0 ){
		shutdown( HANDLE,SHUT_RDWR );
		close( HANDLE );
		HANDLE = (-1);
	}
}

void cSocket::setTimeout( int second )
{
	if( HANDLE < 0 ) return;

	struct timeval tv;

	tv.tv_sec = second;
	tv.tv_usec = 0;
	setsockopt( HANDLE,SOL_SOCKET,SO_SNDTIMEO,(char *)&tv,sizeof(tv) );
	setsockopt( HANDLE,SOL_SOCKET,SO_RCVTIMEO,(char *)&tv,sizeof(tv) );
}

int cSocket::write( const uint8_t *data,size_t size )
{
	int ret = 0;

	if( HANDLE < 0 || !data || size <= 0 ) return 0; //NOP

	if( H_SSL ) ret = SSL_write( H_SSL,data,size );
	else ret = ::write( HANDLE,data,size );

	return ret;
}

int cSocket::print( const char *fmt, ... )
{
	int ret = 0; va_list va; char *ptr = NULL;

	if( HANDLE < 0 ) return ret; //NOP

	va_start( va,fmt );
	ret = vasprintf( &ptr,fmt,va );
	va_end( va );

	if( ret >= 0 && ptr ){
		ret = write( (uint8_t*)ptr,ret );
		free( ptr );
	}else ret = 0;

	return ret;
}

void cSocket::read( void )
{
	char fnm[128],buf[1025],*ret_data; size_t ret_size,len; FILE *fp;

	if( HANDLE < 0 ) return;

	if( CFILE ){
		delete CFILE;
		CFILE = NULL;
	}

	//テンポラリファイルのオープン
	sprintf( fnm,"socket_read.%d.tmp",getpid() );

	CFILE = new cFile();
	CFILE->PATH = fnm;

	if( !CFILE->open("wb") ) return;
	while( 1 ){
		if( H_SSL ) len = SSL_read( H_SSL,buf,sizeof(buf) - 1 );
		else len = ::read( HANDLE,buf,sizeof(buf) - 1 );
		if( len > 0 ) fwrite( buf,len,1,CFILE->FP );
		else break;
	}
	CFILE->close();

	CFILE->read();

	remove( fnm );
}

bool cSocket::httpSplit( void )
{
	char *p1,*p2,*p3,*p4; int chunk_size;

	if( !CFILE || !CFILE->DATA ) return false;
	if( (p1 = strstr( (char*)CFILE->DATA,"\r\n\r\n" )) == NULL ) return false; //NO HTTP

	if( HEAD ){
		free( HEAD );
		HEAD = NULL;
		HLEN = 0;
	}
	if( BODY ){
		free( BODY );
		BODY = NULL;
		BLEN = 0;
	}
	
	*(p1 + 2) = '\0';
	HEAD = strdup( (char*)CFILE->DATA );
	*(p1 + 2) = '\r';
	if( !HEAD ) return false;
	HLEN = strlen( HEAD );

	p3 = p1 + 4; //SEEK TO BODY

	if( (p1 = strcasestr( HEAD,"Content-Length: " )) != NULL &&
		(p2 = strchr( p1,'\r' )) != NULL ){
		p1 += 16; *p2 = '\0'; BLEN = atoi( p1 ); *p2 = '\r';
		if( (BODY = (uint8_t*)malloc( BLEN + 1 )) != NULL ){
			memcpy( BODY,p3,BLEN ); BODY[BLEN] = '\0';
		}else BLEN = 0;
	}
	else if( strcasestr( HEAD,"Transfer-Encoding: chunked" ) ){
		if( (BODY = (uint8_t*)malloc( CFILE->SIZE )) != NULL ){ //最大確保
		for( ; (p4 = strstr( p3,"\r\n" )) != NULL; p3 = p4 ){
			*p4 = '\0'; chunk_size = 0;
			if( strlen( p3 ) > 0 ) sscanf( p3,"%x",&chunk_size );
			*p4 = '\r'; p4 += 2;
			if( chunk_size > 0 ){
				memcpy( BODY + BLEN,p4,chunk_size );
				BLEN += chunk_size; BODY[BLEN] = '\0';
			}
			p4 += chunk_size;
		}}
	}

	return (BODY && BLEN > 0 ? true:false);
}

