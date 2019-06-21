// 配列
//
#ifndef __cArray_h__
#define __cArray_h__
#include "libcpp.h"

class cArray
{
public:
	std::vector<cData> *ARRAY;

	cArray( void );
	virtual	~cArray( void );
	void init( int count );
	int count( void );
	cData* indexAt( int index );
	cData* snameAt( const char *sname );
	cData* lnameAt( const char *lname );
};

#endif //__cArray_h__
