// ソケット通信
//
#ifndef __LSocket_h__
#define __LSocket_h__
#include "libcpp.h"

#define dSocketTimeout 10

class LSocket
{
public:
	std::string mHost;
	std::string mPort;
	int mHandle;
	SSL_CTX *mSSLCtx;
	SSL *mSSL;
	std::vector<uint8_t> mData;
	std::string mHead;
	std::vector<uint8_t> mBody;

	LSocket( void );
	virtual	~LSocket( void );

	bool connect( void );
	void disconnect( void );
	void setTimeout( int second );
	int writeString( std::string &str );
	void read( void );
	bool split( void );
};

#endif //__LSocket_h__
