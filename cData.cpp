// データ
//
#include "libcpp.h"

cData::cData( void )
{
	SNAME[0] = '\0';
	LNAME = NULL;
	VALUE = 0;
	TEXT = NULL;
	TEXTLEN = 0;
	DATA = NULL;
	DATALEN = 0;
}

cData::~cData( void )
{
	if( LNAME ) free( LNAME );
	if( TEXT ) free( TEXT );
	if( DATA ) free( DATA );
}
