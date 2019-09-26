#ifndef _NO_LDATABASE
// データベース関係
//
#include "LDatabase.h"

LDatabase::LDatabase( std::string &logstr ) :
	mLogStr(logstr)
{
	int i;

	mDBHost = NULL;
	mDBUser = NULL;
	mDBPass = NULL;
	mDBName = NULL;
	mDBPort = 0;

	for( i = 0; i < countof(mDB); i++ ){
		mDB[i] = NULL;
	}
}

LDatabase::~LDatabase( void )
{
	disconnectDB( (-1) );
}

bool LDatabase::connectDB( int db_index )
{
	int i; bool ret = true;
	for( i = 0; i < countof(mDB); i++ ){
		if( i == db_index || db_index < 0 ){
			if( mDB[i] == NULL ){
				mDB[i] = mysql_init( NULL );
				if( !mysql_real_connect(
					mDB[i],
					mDBHost,
					mDBUser,
					mDBPass,
					mDBName,
					mDBPort,
					NULL,0 ) ){
					LTool::strAppend( mLogStr,"%s\n",mysql_error( mDB[i] ) );
					disconnectDB( i );
					ret = false;
				}
				else{
					mysql_query( mDB[i],"SET NAMES utf8" );
				}
			}
		}
	}

	return ret;
}

void LDatabase::disconnectDB( int db_index )
{
	int i;
	for( i = 0; i < countof(mDB); i++ ){
		if( i == db_index || db_index < 0 ){
			if( mDB[i] != NULL ){
				mysql_close( mDB[i] );
				mDB[i] = NULL;
			}
		}
	}
}

MYSQL_RES* LDatabase::readDB( int db_index,std::string query )
{
	if( db_index < 0 || db_index >= countof(mDB) ) return NULL;
	if( !connectDB( db_index ) ) return NULL;

	LTool::strAppend( mLogStr,"%s\n",query.c_str() );

	if( mysql_query( mDB[db_index],query.c_str() ) != 0 ){
		LTool::strAppend( mLogStr,"%s\n",mysql_error( mDB[db_index] ) );
		return NULL;
	}

	return mysql_use_result( mDB[db_index] );
}

bool LDatabase::writeDB( int db_index,std::string query )
{
	if( db_index < 0 || db_index >= countof(mDB) ) return NULL;
	if( !connectDB( db_index ) ) return NULL;

	LTool::strAppend( mLogStr,"%s\n",query.c_str() );

	if( mysql_query( mDB[db_index],query.c_str() ) != 0 ){
		LTool::strAppend( mLogStr,"%s\n",mysql_error( mDB[db_index] ) );
		return false;
	}

	return true;
}

int LDatabase::countDB( int db_index,std::string query )
{
	MYSQL_RES* res = readDB( db_index,query );
	if( !res ) return 0; //not found

	int ret = 0;
	MYSQL_ROW row = mysql_fetch_row( res );
	if( row ){
		ret = atoi( row[0] );
	}
	mysql_free_result( res );

	LTool::strAppend( mLogStr,"HIT COUNT %d\n",ret );
	return ret;
}

const char* LDatabase::errorDB( int db_index )
{
	if( !mDB[db_index] ) return "";
	return mysql_error( mDB[db_index] );
}

char* LDatabase::escapeDB( int db_index,const char *src )
{
	if( !mDB[db_index] || strnull(src) ) return NULL;

	size_t len = strlen( src );
	char *dst = (char*)malloc( len * 2 + 32 );

	mysql_real_escape_string( mDB[db_index],dst,src,len );

	return dst;
}

#endif //_NO_LDATABASE
