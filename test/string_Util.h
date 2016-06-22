#ifndef STRING_UTIL_H
#define STRING_UTIL_H
#include <string.h>
#include <string>
#include <vector>
#include <iconv.h>


int _utf16_to_utf8(std::string &src, std::string &result);
int _utf8_to_utf16(std::string &src, std::string &result);
int _utf16_to_gbk(std::string &src, std::string &result);
int _gbk_to_utf16(std::string &src, std::string &result);
int _gbk_to_utf16le(std::string &src, std::string &result);
int _utf8_to_gbk(std::string &src, std::string &result);
int _gbk_to_utf8(std::string &src, std::string &result);
int gbk2utf8(char * in, int * inlen,char * out, int * outlen);

//std::string strim
std::string trim_left(std::string &s,std::string &drop);
std::string trim(std::string &s,std::string &drop);
std::string trimall(std::string &src);
//std::string trim_line1(std::string &s);
//std::string trim_line2(std::string &s);
//std::string trim_line(std::string &s);

std::string cut_strLen(std::string &src,int len);
std::string  str_find(std::string &src,int ibeg,std::string begtag,std::string endtag,int &ifind);

void toUpper(std::basic_string<char>& s);
void toLower(std::basic_string<char>& s);
std::string ToDBS(std::string str);

char * cltrim ( char * sLine );
char * crtrim ( char * sLine );
char * ctrim ( char * sLine );

int strtoks(const char *src,const char *sep,std::vector<std::string> &ret);
int separateIP(const char *value, char *ip, int *port);

uint64_t hex2uint64(std::string hexStr);

//hashËã·¨º¯Êı

unsigned int _sgHash(const void *key, int len);
//url process
#endif
