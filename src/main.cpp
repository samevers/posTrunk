//#include "Platform/encoding.h"
//#include "Platform/gchar.h"
//#include "WordSegmentor4/IQSegmentor/QuerySegmentor.h"
//#include "Platform/encoding/support.h"
//#include "Platform/encoding/EncodingConvertor.h"
//#include "Platform/bchar.h"
//#include "WordSegmentor4/Segmentor/Graph.h"
//#include "string_Util.h"
//#include "Platform/log.h"
//#include "Platform/bchar_cxx.h"
//#include "utils/poolbuf_allocator/poolbuf_allocator.hpp"
//#include "utils/poolbuf_allocator/PoolBufAhoCorasick.h"
#include "string_Util.h"
#include "Jb.hpp"
#include "KeywordExtractor.hpp"
#include <iostream>
#include <string>
using namespace std;


const char* const DICT_PATH = "data/base/analysis/queryclass/posTag/posTag.dict.utf8";
const char* const HMM_PATH = "data/base/analysis/queryclass/posTag/hmm_model.utf8";
const char* const USER_DICT_PATH = "data/base/analysis/queryclass/posTag/user.dict.utf8";
const char* const IDF_PATH = "data/base/analysis/queryclass/posTag/idf.utf8";
const char* const STOP_WORD_PATH = "data/base/analysis/queryclass/posTag/stop_words.utf8";
const static string POS_TAG_FILE = "posTag.map";


cppjb::Jb jb;
int posTag(const char* query, string& query_qj, 
		vector<string>& word_vec, vector<pair<string,string> >& pos_vec)
{
	string s = query_qj;
	string input = query;
	string output;
	output = input;
	_gbk_to_utf8(input, output);
	pos_vec.clear();
	jb.Tag(output, pos_vec);
	for(int i = 0; i < pos_vec.size(); i++)
	{
		_utf8_to_gbk(pos_vec[i].second, output);
		cout << output << " ";
	}
	cout << endl;

	return 0;
}
int main()
{
	jb.loadDic(DICT_PATH, HMM_PATH, USER_DICT_PATH);
	vector<string> word_vec;
	vector<pair<string,string> > pos_vec;

	//timeval before_request_decode;
	//gettimeofday(&before_request_decode, NULL);
	string line;
	while(getline(cin, line))
	{
		if(posTag(line.c_str(), line, word_vec, pos_vec) == -1)
			;
	}

	return 0;
}
