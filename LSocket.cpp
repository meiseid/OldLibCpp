// ソケット通信
//
#include "libcpp.h"

LSocket::LSocket( void )
{
	mHandle = (-1);
	mSSLCtx = NULL;
	mSSL = NULL;
}

LSocket::~LSocket( void )
{
	disconnect();
}

bool LSocket::connect( void )
{
	if( mHost.empty() || mPort.empty() ) return false;

	disconnect();

	struct sockaddr_in addr; struct hostent *servhost; int ret;
	unsigned short rand_ret;

	servhost = gethostbyname( mHost.c_str() );
	if( (mHandle = socket( AF_INET,SOCK_STREAM,0 )) >= 0 ){
		addr.sin_family = AF_INET;
		if( servhost ) memcpy( (char*)&addr.sin_addr,servhost->h_addr,servhost->h_length );
		else addr.sin_addr.s_addr = inet_addr( mHost.c_str() );	
		addr.sin_port = htons( std::stoi( mPort ) );
		setTimeout( DSocketTimeout );
		ret = ::connect( mHandle,(struct sockaddr *)&addr,sizeof(addr) );
		if( ret >= 0 ){
			if( mPort == "443" ){
				SSL_load_error_strings();
				SSL_library_init();
				if( (mSSLCtx = SSL_CTX_new( SSLv23_client_method() )) != NULL ){
					if( (mSSL = SSL_new( mSSLCtx )) != NULL ){
						if( (ret = SSL_set_fd( mSSL,mHandle )) != 0 ){
							RAND_poll();
							while( RAND_status() == 0 ){
								rand_ret = rand() % 65536;
								RAND_seed( &rand_ret,sizeof(rand_ret) );
							}
							if( (ret = SSL_connect( mSSL )) == 1 ){
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

void LSocket::disconnect( void )
{
	if( mSSL ){
		SSL_shutdown( mSSL );
		SSL_free( mSSL );
		mSSL = NULL;
	}

	if( mSSLCtx ){
		SSL_CTX_free( mSSLCtx );
		mSSLCtx = NULL;
	}

	if( mHandle >= 0 ){
		shutdown( mHandle,SHUT_RDWR );
		close( mHandle );
		mHandle = (-1);
	}
}

void LSocket::setTimeout( int second )
{
	if( mHandle < 0 ) return;

	struct timeval tv;

	tv.tv_sec = second;
	tv.tv_usec = 0;
	setsockopt( mHandle,SOL_SOCKET,SO_SNDTIMEO,(char *)&tv,sizeof(tv) );
	setsockopt( mHandle,SOL_SOCKET,SO_RCVTIMEO,(char *)&tv,sizeof(tv) );
}

int LSocket::writeString( std::string &str )
{
	int ret = 0;

	if( mHandle < 0 ) return 0; //NOP

	if( mSSL ) ret = SSL_write( mSSL,str.c_str(),str.length() );
	else ret = ::write( mHandle,str.c_str(),str.length() );

	return ret;
}

void LSocket::read( void )
{
	char fnm[128],buf[1025],*ret_data; size_t ret_size,len; FILE *fp;

	if( mHandle < 0 ) return;

	size_t capacity = 500000; //とりあえず500kbyte

	mData.clear();
	mData.reserve( capacity );

	while( 1 ){
		if( mSSL ) len = SSL_read( mSSL,buf,sizeof(buf) - 1 );
		else len = ::read( mHandle,buf,sizeof(buf) - 1 );
		if( len > 0 ){
			if( mData.size() + len > mData.capacity() ){
				mData.reserve( mData.capacity() + capacity );
			}
			mData.resize( mData.size() + len ); //先にresizeしないとeofを伸ばしてくれない
			memcpy( mData.data() + (mData.size() - len),buf,len );
		}
		else break;
	}
}

bool LSocket::split( void )
{
	char *p1,*p2,*p3; int tmp_len;

	if( mData.size() <= 0 ) return false;

	char *src = (char*)mData.data();

	if( (p1 = strstr( src,"\r\n\r\n" )) == NULL ) return false; //NO HTTP

	mHead.clear();
	mBody.clear();
	mBody.resize( mData.size() );

	char *dst = (char*)mBody.data();
	size_t dsl = 0;

	*p1 = '\0';
	mHead.append( src );

	if( (p2 = strcasestr( src,"Content-Length: " )) != NULL &&
		(p3 = strchr( p2,'\r' )) != NULL ){
		p2 += 16;
		*p3 = '\0';
		tmp_len = atoi( p2 );
		*p3 = '\r';
		p1 += 4; //skip \r\n\r\n
		memcpy( dst,p1,tmp_len );
		dsl = tmp_len;
	}
	else if( strcasestr( src,"Transfer-Encoding: chunked" ) ){
		p1 += 2; //skip \r\n
		for( ; (p2 = strstr( p1,"\r\n" )) != NULL &&
			(p3 = strstr( p2 + 2,"\r\n" )) != NULL; p1 = p3 ){
			p2 += 2; //skip \r\n
			*p3 = '\0';
			sscanf( p2,"%x",&tmp_len );
			*p3 = '\r';
			p3 += 2; //skip \r\n
			memcpy( dst + dsl,p3,tmp_len );
			dsl += tmp_len;
			p3 += tmp_len;
		}
	}

	mBody.resize( dsl );
	return mBody.size() > 0 ? true:false;
}
