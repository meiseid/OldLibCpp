// ソケット通信
//
#ifndef __cSocket_h__
#define __cSocket_h__
#include "libcpp.h"

class cSocket
{
public:
	char *HOST;
	char *PORT;
	int HANDLE;
	SSL_CTX *H_SSL_CTX;
	SSL *H_SSL;
	cFile *CFILE;
	char *HEAD;
	size_t HLEN;
	uint8_t *BODY;
	size_t BLEN;

	cSocket( void );
	virtual	~cSocket( void );

	bool connect( void );
	void disconnect( void );
	void setTimeout( int second );
	int write( const uint8_t *data,size_t size );
	int print( const char *fmt, ... );
	void read( void );
	bool httpSplit( void );
};

#endif //__cSocket_h__
