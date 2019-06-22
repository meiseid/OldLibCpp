// テストメイン
//
#include "libcpp.h"

int main( int argc,char **argv )
{
/*
	LFile f = LFile( "cArray.h" );
	f.readDataFile();
	std::string s = std::string((const char*)f.mData.data());

	std::cout << s;

//	printf( "%d\n",f.mData.size() );

	LFile wf = LFile("test.txt");
	wf.open( "wb" );
	fprintf( wf.mFp,"ああああああ\n" );
	fprintf( wf.mFp,"ああああああ\n" );
	fprintf( wf.mFp,"ああああああ\n" );
	fprintf( wf.mFp,"ああああああ\n" );
	wf.close();
*/
	std::vector<SParam> params = std::vector<SParam>();

	LTool::readConfigFile( "../NorikaeMini/cgi/norimini.conf",params );

	int i,n;

	for( i = 0,n = params.size(); i < n; i++ ){
		SParam &param = params.at( i );
		printf( "%d=%s,%s\n",i,param.mName.c_str(),param.mText.c_str() );

	}

	char *ptr,*mem = NULL; size_t len = 0;

	if( (ptr = getenv( "QUERY_STRING" )) != NULL && (len = strlen( ptr )) > 0 ){
		mem = strdup( ptr );
	}else if( (ptr = getenv( "CONTENT_LENGTH" )) != NULL && (len = atoi( ptr )) > 0 ){
		if( (mem = (char*)malloc( len + 1 )) != NULL ){
			fread( mem,len,1,stdin ); mem[len] = '\0';
		}
	}
	if( mem ){
		params.clear();
		LTool::readSinglePart( mem,len,'&',params );
		for( i = 0,n = params.size(); i < n; i++ ){
			SParam &param = params.at( i );
			printf( "%d=%s,%s\n",i,param.mName.c_str(),param.mText.c_str() );
		}
	}

	std::string aaa = std::string("落+合 南/長a崎*");
	std::string bbb = std::string();
	std::string ccc = std::string();
	LTool::urlEncode(aaa,bbb);
	LTool::urlDecode(bbb,ccc);
	std::cout << aaa << "," << bbb << "," << ccc << "\n";

	std::string vc = "あ";
	std::cout << "len=" << vc.length() << "\n";

	std::vector<uint8_t> vct = std::vector<uint8_t>();
	vct.reserve( 1000000 );
	vct.resize( 4000 );
	std::cout << "capacity=" << vct.capacity() << "\n";
	std::cout << "size=" << vct.size() << "\n";
	vct.reserve( 500 );
	vct.resize( 100 );
	std::cout << "capacity=" << vct.capacity() << "\n";
	std::cout << "size=" << vct.size() << "\n";


	std::string res = std::string(
	"Content-VVV:\r\n"
	"\r\n"
	"BODY\r\n"
	"TEST\r\n"
	);
	size_t p1 = res.find("\rvvv\n\r\n");
	if( p1 == std::string::npos ){
		std::cout << "not found npos\n";
	}
	else{
		std::cout << "finding: " << p1 << "\n";
	}

	char mtm[32];

	LTool::strFullTime( mtm );
	std::cout << mtm << " begin sock\n";

	LSocket sock = LSocket();

	sock.mHost = "imbt.jorudan.co.jp";
	sock.mPort = "443";

	std::string noripara = std::string(
	"GET /exp/norijson.cgi?incs=utf8&c=10&p=0&plusmode=0&xpd=0&f=%E8%90%BD%E5%90%88%E5%8D%97%E9%95%B7%E5%B4%8E&t=%E7%B7%B4%E9%A6%AC&k1=%E6%96%B0%E5%AE%BF&stp=0&d=20191201&tm=0400&ft=0 HTTP/1.1\r\n"
	"Host: imbt.jorudan.co.jp\r\n"
	"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:67.0) Gecko/20100101 Firefox/67.0\r\n"
	"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
	"Accept-Language: ja,en-US;q=0.7,en;q=0.3\r\n"
	"Accept-Encoding: gzip, deflate, br\r\n"
	"Connection: close\r\n"
	"\r\n"
	);

	std::cout << noripara;

	if( sock.connect() ){
		LTool::strFullTime( mtm );
		std::cout << mtm << " connected sock\n";

		sock.writeString( noripara );

		LTool::strFullTime( mtm );
		std::cout << mtm << " sent request\n";

		sock.read();

		LTool::strFullTime( mtm );
		std::cout << mtm << " receive reply\n";

		if( sock.split() ){

			LTool::strFullTime( mtm );
			std::cout << mtm << " splitted reply\n";

			std::cout << sock.mHead << "\n";

			std::string tst = std::string();
			tst.append( (char*)sock.mBody.data(),sock.mBody.size() );
			std::cout << "tst len=" << tst.length() << ",val=" << tst;

			//std::cout << sock.mStrBody << "\n";

			LTool::strFullTime( mtm );
			std::cout << mtm << " output complete\n";
		}

	}

	return 0;
}
