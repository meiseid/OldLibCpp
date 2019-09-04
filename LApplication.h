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
	std::vector<SParam> mCookie;

	struct tm mDate;
	time_t mTime;
	char* mDateStr;

	std::string mClientIp;

	std::string mOutHead;
	std::string mOutBody;

	LApplication( int argc,char **argv );
	LApplication( void );
	virtual	~LApplication( void );
	virtual void run( void );
	virtual void finish( void );
	void readCgiParams( void );
	void readCookieParams( void );
};

#endif //__LApplication_h__
