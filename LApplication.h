// アプリケーション
//
#ifndef __LApplication_h__
#define __LApplication_h__
#include "libcpp.h"

class LApplication
{
public:
	std::string mLogPath;
	std::string mLogStr;

	std::vector<SParam> mConfig;
	std::vector<SParam> mCgi;

	struct tm mDate;
	char* mDateStr;

	std::string mOutHead;
	std::string mOutBody;

	LApplication( int argc,char **argv );
	LApplication( void );
	virtual	~LApplication( void );
	virtual void run( void );
	virtual void finish( void );
	void readCgiParams( void );
};

#endif //__LApplication_h__
