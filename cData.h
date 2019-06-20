// データ
//
#ifndef __cData_h__
#define __cData_h__
#include "libcpp.h"

class cData
{
public:
	char SNAME[MAX_SHORT_NAME];
	char *LNAME;
	int VALUE;
	char *TEXT;
	size_t TEXTLEN;
	uint8_t *DATA;
	size_t DATALEN;

	cData( void );
	virtual	~cData( void );
};

#endif //__cData_h__
