// 静的ツール
//
#include "libcpp.h"

int LTool::sizeFile( const char *path,time_t *mtime )
{
	struct stat status;

	if( mtime ) *mtime = 0;

	if( strnull(path) ) return (-1);

	if( stat( path, &status ) == 0 ){
		if( mtime ) *mtime = status.st_mtime;
		return status.st_size; //file size
	}
	return (-1); //not exists
}

void LTool::urlDecode( std::string &src,std::string &dst )
{
	if( src.empty() ) return; //NOP
	int i,num; char buf[8];
	const char *c_src = src.c_str();

	for( i = 0; c_src[i] != '\0'; i++ ){
		if( c_src[i] == '%' ){
			if( c_src[i + 1] == '\0' || c_src[i + 2] == '\0' ){
				break; //overflow
			}
			memcpy( buf,c_src + i,3 ); buf[3] = '\0';
			sscanf( buf,"%%%02X",&num );
			if( num > 255 ){
				break; //overflow
			}
			dst.append( 1,(char)num );
			i += 2;
		}
		else if( c_src[i] == '+' ){
			dst.append( 1,' ' );
		}
		else{
			dst.append( 1,c_src[i] );
		}
	}
}

void LTool::urlEncode( std::string &src,std::string &dst )
{
	if( src.empty() ) return; //NOP
	int i; char buf[8];
	const char *c_src = src.c_str();

	for( i = 0; c_src[i] != '\0'; i++ ){
		if( (c_src[i] >= '0' && c_src[i] <= '9') ||
			(c_src[i] >= 'A' && c_src[i] <= 'Z') ||
			(c_src[i] >= 'a' && c_src[i] <= 'z') ||
			c_src[i] == '_' || c_src[i] == '-' || c_src[i] == '.' ){
			dst.append( 1,c_src[i] );
		}
		else if( c_src[i] == ' ' ){
			dst.append( 1,'+' );
		}
		else{
			sprintf( buf,"%%%02X",(unsigned char)(c_src[i]) );
			dst.append( buf,3 );
		}
	}
}

void LTool::xmlDecode( std::string &src,std::string &dst )
{
	if( src.empty() ) return; //NOP

	dst.append(src);
	strReplace( dst,"&amp;","&" );
	strReplace( dst,"&quot;","\"" );
	strReplace( dst,"&lt;","<" );
	strReplace( dst,"&gt;",">" );
	strReplace( dst,"&#x27;","'" );
	strReplace( dst,"&#x60;","`" );
}

void LTool::xmlEncode( std::string &src,std::string &dst )
{
	if( src.empty() ) return; //NOP
	dst.append(src);
	strReplace( dst,"&","&amp;" );
	strReplace( dst,"\"","&quot;" );
	strReplace( dst,"<","&lt;" );
	strReplace( dst,">","&gt;" );
	strReplace( dst,"'","&#x27;" );
	strReplace( dst,"`","&#x60;" );
}

std::string LTool::readTextFile( const char *path )
{
	std::string reply = std::string();
	LFile file = LFile();
	file.mPath.append( path );
	file.readTextFile();
	if( !file.mText.empty() ){
		reply.append( file.mText );
	}
	return reply;
}

void LTool::readConfigFile( std::string path,std::vector<SParam> &params )
{
	char *mem,*p1,*p2,*p3,*p4; int i,n; size_t len; SParam param;

	LFile file = LFile();
	file.mPath = path;
	file.readTextFile();
	if( file.mText.empty() ) return; //read fail
	mem = (char*)file.mText.c_str();

	for( i = 0,p1 = mem; (p2 = strstr( p1,"=\"" )) != NULL; p1 = p2 + 2 ){
		if( p1 == p2 || *(p2 + 2) == '\0' || *(p2 + 2) == '"' ) continue;
		for( p3 = p2 - 1; p3 != mem && *p3 != '\r' && *p3 != '\n'; p3-- ){ ; }
		if( p3 != mem ) p3++; if( *p3 == '#' ) continue;
		if( (p4 = strchr( p2 + 2,'"' )) == NULL ) p4 = strchr( p2 + 2,'\0' );
		len = (size_t)p2 - (size_t)p3;
		if( len <= 0 ) continue;
		param.mName.clear();
		param.mName.append( p3,len );
		p2 += 2; len = (size_t)p4 - (size_t)p2;
		if( len <= 0 ) continue;
		param.mText.clear();
		param.mText.append( p2,len );
		params.push_back( param );
	}
}

void LTool::readSinglePart( char *mem,size_t len,char sep,std::vector<SParam> &params )
{
	char *fr,*to,*bg,*en; size_t k_len; int i; SParam param;
	std::string ts = std::string();

	for( i = 0,fr = mem; (to = strchr( fr,'=' )) != NULL; fr = to + 1 ){
		k_len = (size_t)to - (size_t)fr;
		bg = to + 1;
		if( (en = strchr( bg,sep )) != NULL ){ *(to = en) = '\0'; }
		else{ en = mem + len; to = en - 1; }
		if( bg[0] != '\0' && k_len > 0 ){ 
			param.mName.clear();
			param.mName.append( fr,k_len );
			ts.clear();
			ts.append( bg );
			param.mText.clear();
			urlDecode( ts,param.mText );
			params.push_back( param );
		}
		if( *(to + 1) == ' ' ) to++;
	}
}

char* LTool::paramText( const char *key,const std::vector<SParam> &params )
{
	for( int i = 0,n = params.size(); i < n; i++ ){
		if( strcasecmp( params.at( i ).mName.c_str(),key ) == 0 ){
			return (char*)params.at( i ).mText.c_str();
		}
	}
	return NULL;
}

std::string LTool::clockText( std::string label )
{
	time_t tm; struct tm date; struct timeval tv; int msec; char tstr[128];

	tm = time( NULL );
	date = *(localtime( &tm ));
	gettimeofday( &tv,NULL );
	msec = (int)(tv.tv_usec / 1000);

	sprintf( tstr,"%04d-%02d-%02d %02d:%02d:%02d.%03d ",
		date.tm_year + 1900,date.tm_mon + 1,
		date.tm_mday,date.tm_hour,date.tm_min,date.tm_sec,msec );

	std::string ret( tstr );
	ret.append( label );
	ret.append( "\n" );
	return ret;
}

void LTool::getDateText( const struct tm *src,char *dst,size_t len )
{
	if( !src || !dst ) return; //NOP
	if( len <= 0 ) len = 14; //Default

	static const char *mons[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
	static const char *week[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};

	if( len == 32 || len == 34 ){ //for cookie or rss-pubDate
		struct tm gmt; memcpy( &gmt,src,sizeof(gmt) );
		if( len == 32 ){ //cookie
			gmt.tm_hour -= 9; mktime( &gmt );
			sprintf( dst,"%s, %d-%s-%04d %02d:%02d:%02d GMT",week[gmt.tm_wday],
				gmt.tm_mday,mons[gmt.tm_mon],gmt.tm_year + 1900,gmt.tm_hour,gmt.tm_min,gmt.tm_sec );
		}
		else{ //rss-pubDate
			sprintf( dst,"%s, %02d %s %04d %02d:%02d:%02d +0900",week[gmt.tm_wday],
				gmt.tm_mday,mons[gmt.tm_mon],gmt.tm_year + 1900,gmt.tm_hour,gmt.tm_min,gmt.tm_sec );
		}
	}
	else if( len == 10 ) sprintf( dst,"%04d-%02d-%02d",src->tm_year + 1900,src->tm_mon + 1,src->tm_mday );
	else if( len == 14 ) sprintf( dst,"%04d%02d%02d%02d%02d%02d",src->tm_year + 1900,src->tm_mon + 1,
		src->tm_mday,src->tm_hour,src->tm_min,src->tm_sec );
	else sprintf( dst,"%04d-%02d-%02d %02d:%02d:%02d",src->tm_year + 1900,src->tm_mon + 1,
		src->tm_mday,src->tm_hour,src->tm_min,src->tm_sec );
}

int LTool::strAppend( std::string &str,const char *fmt, ... )
{
	int ret = 0; va_list va; char *ptr = NULL;

	va_start( va,fmt );
	ret = vasprintf( &ptr,fmt,va );
	va_end( va );

	if( ret >= 0 && ptr ){
		str.append( ptr );
		free( ptr );
	}else ret = 0;

	return ret;
}

void LTool::strReplace( std::string &str,std::string from,std::string to )
{
	std::string::size_type pos( str.find( from ) );

	while( pos != std::string::npos ){
		str.replace( pos,from.length(),to );
		pos = str.find( from,pos + to.length() );
	}
}

int LTool::strSplit( const char *src,char key,std::vector<std::string> &dst )
{
	int counts = 0;
	dst.clear();

	if( strnull(src) ) return counts;

	char *p1,*p2; size_t len;

	for( p1 = (char*)src; (p2 = strchr( p1,key )) != NULL ||
		(p2 = strchr( p1,'\0' )) != NULL; p1 = p2 + 1 ){
		len = (size_t)p2 - (size_t)p1;
		if( len > 0 ){
			std::string as = std::string( p1,len );
			dst.push_back( as );
			counts++;
		}
		if( *p2 == '\0' ) break;
	}
	return counts;
}

void LTool::makeCostString( int cost_num,char *cost_str )
{
	char cost[32 + 1],tsoc[32 + 1];
	int i,j,k; size_t len;

	len = sprintf( cost,"%d",cost_num );

	for( i = 0,k = 1,j = (len - 1); j >= 0; i++,k++,j-- ){
		tsoc[i] = cost[j];
		if( k % 3 == 0 && (j - 1) >= 0 ) tsoc[++i] = ',';
	}tsoc[i] = '\0';

	len = strlen( tsoc );
	for( i = 0,j = (len - 1); j >= 0; i++,j-- ){
		cost_str[i] = tsoc[j];
	}
	cost_str[i] = '\0';
}

//全角を2、半角を1とした長さ計算
int LTool::widthText( const char *text )
{
	if( !text ) return 0;
	int i,j;
	for( i = 0,j = 0; text[i] != '\0'; i++ ){
		if( (unsigned char)text[i] >= 0x80 ){
			j += 2;
			if( (unsigned char)text[i] >= 0xE0 ) i += 2;
			else if( (unsigned char)text[i] != 0x5C ) i++;
		}else j++;
	}
	return j; //String Width
}

//省略文字列の作成 全角を2、半角を1として扱う
//HTMLタグ囲い(<～>)はスキップする
//ccr:改行文字の変換文字 trm:終端省略文字
void LTool::trimingString( const char *src,char *dst,int lim,const char *ccr,const char *trm )
{
	int i,j,now,btag,add,adk;

	if( !src || !dst ) return;
	for( i = 0,j = 0,now = 0,btag = 0,add = 0,adk = 0,dst[0] = '\0'; src[i] != '\0'; i++ ){
		if( (unsigned char)src[i] >= 0x80 ){ //全角
			if( (unsigned char)src[i] >= 0xE0 ){ //3byte
				i += 2;
				if( btag ) continue;
				if( now + 2 > lim ) break;
				dst[j++] = src[i - 2]; dst[j++] = src[i - 1]; dst[j++] = src[i]; dst[j] = '\0'; now += 2;
				if( strncmp( &(src[i - 2]),"。",strlen( "。" ) ) == 0 ) adk = j;
				add = 3;
			}
			else{ //2byte
				i++;
				if( btag ) continue;
				if( now + 2 > lim ) break;
				dst[j++] = src[i - 1]; dst[j++] = src[i]; dst[j] = '\0'; now += 2;
				add = 2;
			}
		}
		else{ //半角
			if( btag ){
				if( src[i] == '>' ) btag = 0;
				continue;
			}
			if( src[i] == '<' ){
				btag = 1;
				continue;
			}
			if( src[i] == '\r' && src[i + 1] == '\n' ){
				continue;
			}
			if( src[i] == '\r' || src[i] == '\n' ){
				adk = j;
				if( !ccr ) continue;
				if( (unsigned char)ccr[0] >= 0x80 ){
					if( now + 2 > lim ) break;
					strcat( dst,ccr ); j += strlen( ccr ); now += 2;
					add = strlen( ccr );
				}
				else{
					if( now + 1 > lim ) break;
					strcat( dst,ccr ); j += strlen( ccr ); now++;
					add = strlen( ccr );
				}
			}else{
				if( now + 1 > lim ) break;
				dst[j++] = src[i]; dst[j] = '\0'; now++;
				add = 1;
			}
		}
	}
	if( src[i] != '\0' && add ){
		if( trm ) strcpy( &(dst[j - add]),trm );
		else if( adk > 0 ) dst[adk] = '\0';
		else strcpy( &(dst[j - add]),"..." );
	}
}

static struct{ char han; const char *zen; }gHanZenTbl[] = {
	{' ',"　"},{'!',"！"},{'"',"”"},{'#',"＃"},{'$',"＄"},{'%',"％"},{'&',"＆"},{'\'',"’"},
	{'(',"（"},{')',"）"},{'*',"＊"},{'+',"＋"},{',',"、"},{'-',"－"},{'.',"。"},{'/',"／"},
	{':',"："},{';',"；"},{'<',"＜"},{'=',"＝"},{'>',"＞"},{'?',"？"},{'@',"＠"},{'[',"［"},
	{'\\',"￥"},{']',"］"},{'^',"＾"},{'_',"＿"},{'`',"‘"},{'{',"｛"},{'|',"｜"},{'}',"｝"},{'~',"～"}
};

//可能な限りの半角→全角変換
char* LTool::hankakuToZenkaku( const char *src,char *dst )
{
	int i,j,x,y;
	if( dst ) dst[0] = '\0';
	if( strnull(src) ) return NULL;
	if( !dst && (dst = (char*)malloc( strlen( src ) * 3 + 1 )) == NULL ) return NULL;

	for( i = 0,j = 0; src[i] != '\0'; i++ ){
		if( (unsigned char)src[i] >= 0x80 ){ //全角
			if( (unsigned char)src[i] >= 0xE0 ){
				dst[j++] = src[i];
				dst[j++] = src[i + 1];
				dst[j++] = src[i + 2];
				i += 2;
			}else if( (unsigned char)src[i] != 0x5C ){
				dst[j++] = src[i];
				dst[j++] = src[i + 1];
				i++;
			}else{
				dst[j++] = src[i];
			}
		}else{ //半角現る
			if( src[i] >= 'A' && src[i] <= 'Z' ){
				dst[j++] = 0xEF;
				dst[j++] = 0xBC;
				dst[j++] = 0xA1 + (src[i] - 'A');
			}
			else if( src[i] >= 'a' && src[i] <= 'z' ){
				dst[j++] = 0xEF;
				dst[j++] = 0xBD;
				dst[j++] = 0x81 + (src[i] - 'a');
			}
			else if( src[i] >= '0' && src[i] <= '9' ){
				dst[j++] = 0xEF;
				dst[j++] = 0xBC;
				dst[j++] = 0x90 + (src[i] - '0');
			}
			else{
				for( x = 0; x < countof(gHanZenTbl); x++ ){
					if( src[i] == gHanZenTbl[x].han ){
						y = strlen( gHanZenTbl[x].zen );
						memcpy( dst + j,gHanZenTbl[x].zen,y );
						j += y; break;
					}
				}
				if( x >= countof(gHanZenTbl) ) dst[j++] = src[i]; //GIVE UP
			}
		}
	}dst[j] = '\0';
	return dst;
}

//可能な限りの全角→半角変換
char* LTool::zenkakuToHankaku( const char *src,char *dst )
{
	int i,j,x;
	if( dst ) dst[0] = '\0';
	if( strnull(src) ) return NULL;
	if( !dst && (dst = (char*)malloc( strlen( src ) + 1 )) == NULL ) return NULL;

	for( i = 0,j = 0; src[i] != '\0'; i++ ){
		if( (unsigned char)src[i] < 0x80 ){ //半角
			dst[j++] = src[i]; continue;
		}
		if( (unsigned char)src[i] == 0xEF &&
			(unsigned char)src[i + 1] == 0xBC &&
			(unsigned char)src[i + 2] >= 0x90 &&
			(unsigned char)src[i + 2] <= 0x99 ){
			dst[j++] = '0' + ((unsigned char)src[i + 2] - 0x90);
			i += 2; continue;
		}
		if( (unsigned char)src[i] == 0xEF &&
			(unsigned char)src[i + 1] == 0xBC &&
			(unsigned char)src[i + 2] >= 0xA1 &&
			(unsigned char)src[i + 2] <= 0xBA ){
			dst[j++] = 'A' + ((unsigned char)src[i + 2] - 0xA1);
			i += 2; continue;
		}
		if( (unsigned char)src[i] == 0xEF &&
			(unsigned char)src[i + 1] == 0xBD &&
			(unsigned char)src[i + 2] >= 0x81 &&
			(unsigned char)src[i + 2] <= 0x9A ){
			dst[j++] = 'a' + ((unsigned char)src[i + 2] - 0x81);
			i += 2; continue;
		}
		for( x = 0; x < countof(gHanZenTbl); x++ ){
			if( strncmp( &(src[i]),gHanZenTbl[x].zen,strlen( gHanZenTbl[x].zen ) ) == 0 ){
				dst[j++] = gHanZenTbl[x].han; break;
			}
		}
		if( (unsigned char)src[i] >= 0xE0 ){
			if( x >= countof(gHanZenTbl) ){
				dst[j++] = src[i];
				dst[j++] = src[i + 1];
				dst[j++] = src[i + 2];
			}i += 2;
		}else{
			if( x >= countof(gHanZenTbl) ){
				dst[j++] = src[i];
				dst[j++] = src[i + 1];
			}i++;
		}
	}dst[j] = '\0';
	return dst;
}

//Base64エンコード
unsigned char* LTool::base64Encode( const unsigned char *str,int length,int *ret_length )
{
	static const char base64_table[] = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
        'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
        'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/', '\0'
	};
	static const char base64_pad = '=';
    	const unsigned char *current = str;
        unsigned char *p;
        unsigned char *result;

        if ((length + 2) < 0 || ((length + 2) / 3) >= (1 << (sizeof(int) * 8 - 2))) {
                if (ret_length != NULL) {
                        *ret_length = 0;
                }
                return NULL;
        }

        result = (unsigned char *)malloc(((length + 2) / 3) * 4 + 128);
        p = result;

        while (length > 2) { /* keep going until we have less than 24 bits */
                *p++ = base64_table[current[0] >> 2];
                *p++ = base64_table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
                *p++ = base64_table[((current[1] & 0x0f) << 2) + (current[2] >> 6)];
                *p++ = base64_table[current[2] & 0x3f];

                current += 3;
                length -= 3; /* we just handle 3 octets of data */
        }

        /* now deal with the tail end of things */
        if (length != 0) {
                *p++ = base64_table[current[0] >> 2];
                if (length > 1) {
                        *p++ = base64_table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
                        *p++ = base64_table[(current[1] & 0x0f) << 2];
                        *p++ = base64_pad;
                } else {
                        *p++ = base64_table[(current[0] & 0x03) << 4];
                        *p++ = base64_pad;
                        *p++ = base64_pad;
                }
        }
        if (ret_length != NULL) {
                *ret_length = (int)(p - result);
        }
        *p = '\0';
        return result;
}

