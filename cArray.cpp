// 配列
//
#include "libcpp.h"

cArray::cArray( int count )
{
	ARRAY = new std::vector<cData>(count);
}

cArray::~cArray( void )
{
	delete ARRAY;
}

cData* cArray::indexAt( int index )
{
	if( index >= ARRAY->size() ) return NULL;
	return &(ARRAY->at( index ));
}

cData* cArray::snameAt( const char *sname )
{
	int i,n;
	for( i = 0,n = ARRAY->size(); i < n; i++ ){
		if( !strnull(ARRAY->at(i).SNAME) &&
			strcmp( ARRAY->at(i).SNAME,sname ) == 0 ){
			return &(ARRAY->at(i));
		}
	}
	return NULL;
}

cData* cArray::lnameAt( const char *lname )
{
	int i,n;
	for( i = 0,n = ARRAY->size(); i < n; i++ ){
		if( !strnull(ARRAY->at(i).LNAME) &&
			strcmp( ARRAY->at(i).LNAME,lname ) == 0 ){
			return &(ARRAY->at(i));
		}
	}
	return NULL;
}


