// 配列
//
#include "libcpp.h"

cArray::cArray( void )
{
	ARRAY = NULL;
}

cArray::~cArray( void )
{
	if( ARRAY ) delete ARRAY;
}

void cArray::init( int count )
{
	if( ARRAY ) delete ARRAY;
	ARRAY = new std::vector<cData>(count);
}

int cArray::count( void )
{
	return ARRAY ? ARRAY->size():0;
}

cData* cArray::indexAt( int index )
{
	if( !ARRAY ) return NULL;
	if( index >= ARRAY->size() ) return NULL;
	return &(ARRAY->at( index ));
}

cData* cArray::snameAt( const char *sname )
{
	if( !ARRAY ) return NULL;
	int i,n;
	for( i = 0,n = ARRAY->size(); i < n; i++ ){
		if( !strnull(ARRAY->at(i).SNAME) &&
			strcasecmp( ARRAY->at(i).SNAME,sname ) == 0 ){
			return &(ARRAY->at(i));
		}
	}
	return NULL;
}

cData* cArray::lnameAt( const char *lname )
{
	if( !ARRAY ) return NULL;
	int i,n;
	for( i = 0,n = ARRAY->size(); i < n; i++ ){
		if( !strnull(ARRAY->at(i).LNAME) &&
			strcasecmp( ARRAY->at(i).LNAME,lname ) == 0 ){
			return &(ARRAY->at(i));
		}
	}
	return NULL;
}
