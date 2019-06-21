// ファイル
//
#include "libcpp.h"

cFile::cFile( void )
{
	PATH = NULL;
	DATA = NULL;
	SIZE = 0;
	FP = NULL;
}

cFile::~cFile( void )
{
	if( DATA ) free( DATA );
	close();
}

bool cFile::open( const char *mode )
{
	struct flock flk;

	if( !PATH || !mode ) return false; //NOP

	close();

	if( (FP = fopen( PATH,mode )) != NULL ){
		flk.l_type	= F_WRLCK;
		flk.l_whence= SEEK_SET;
		flk.l_start	= 0;
		flk.l_len	= 0;
		if( fcntl( fileno( FP ),F_SETLKW,&flk ) != 0 ){
			fclose( FP ); FP = NULL;
		}
	}
	return FP ? true:false;
}

void cFile::close( void )
{
	int ret; struct flock flk;

	if( !FP ) return;

	flk.l_type	= F_UNLCK;
	flk.l_whence= SEEK_SET;
	flk.l_start	= 0;
	flk.l_len	= 0;
	ret = fcntl( fileno( FP ),F_SETLKW,&flk );
	fclose( FP );

	FP = NULL;
}

bool cFile::write( const char *mode,const char *data,size_t size )
{
	if( !data || size <= 0 || !open( mode ) ) return false;

	bool ret = false;

	if( fwrite( data,size,1,FP ) > 0 ){
		ret = true;
	}

	close();

	return ret;
}

void cFile::read( void )
{
	if( strnull(PATH) ) return; //NOP
	if( DATA ){
		free( DATA );
		DATA = NULL;
	}
	SIZE = 0;

	FILE *fp; uint8_t *ptr = NULL; size_t len = 0; int ret = 0;

	if( (fp = fopen( PATH,"rb" )) != NULL ){
		fseek( fp,0L,SEEK_END );
		len = (size_t)ftell( fp );
		if( (ptr = (uint8_t*)malloc( len + 1 )) != NULL ){
			rewind( fp );
			ret = fread( ptr,len,1,fp ); //一気読み
			ptr[len] = '\0';
		}
		fclose( fp );
	}
	if( ret <= 0 ){
		if( ptr ) free( ptr );
		return; //FAILUE
	}

	SIZE = len;
	DATA = ptr;
}

