// ファイル
//
#ifndef __LFile_h__
#define __LFile_h__
#include "libcpp.h"

class LFile
{
public:
	std::string mPath;
	std::string mText;
	std::vector<uint8_t> mData;
	FILE *mFp;

	LFile( void );
	virtual	~LFile( void );
	bool open( const char *mode );
	void close( void );
	void readTextFile( void );
	void readDataFile( void );
};

#endif //__LFile_h__
