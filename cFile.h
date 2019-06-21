// ファイル
//
#ifndef __cFile_h__
#define __cFile_h__
#include "libcpp.h"

class cFile
{
public:
	char *PATH;
	uint8_t *DATA;
	size_t SIZE;
	FILE *FP;

	cFile( void );
	virtual	~cFile( void );

	bool open( const char *mode );
	void close( void );
	bool write( const char *mode,const char *data,size_t size );
	void read( void );

};

#endif //__cFile_h__
