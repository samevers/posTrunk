#include "Jb.hpp"
#include "KeywordExtractor.hpp"
#include "string_Util.h"

using namespace std;

const char* const DICT_PATH = "../dict/posTag.dict.utf8";
const char* const HMM_PATH = "../dict/hmm_model.utf8";
const char* const USER_DICT_PATH = "../dict/user.dict.utf8";
const char* const IDF_PATH = "../dict/idf.utf8";
const char* const STOP_WORD_PATH = "../dict/stop_words.utf8";

int main(int argc, char** argv) {
  cppjb::Jb jb(DICT_PATH,
        HMM_PATH,
        USER_DICT_PATH);
  vector<string> words;
  string result;
  string s = "我是拖拉机学院手扶拖拉机专业的。不用多久，我就会升职加薪，当上CEO，走上人生巅峰。";

  while(cin >> s)
  {
	  if(s == "exit")
	  {
		  return 0;
	  }
  cout << "[demo] Cut With HMM" << endl;
  jb.Cut(s, words, true);
  cout << limonp::Join(words.begin(), words.end(), "/") << endl;

  cout << "[demo] Cut Without HMM " << endl;
  jb.Cut(s, words, false);
  cout << limonp::Join(words.begin(), words.end(), "/") << endl;

  cout << "[demo] CutAll" << endl;
  jb.CutAll(s, words);
  cout << limonp::Join(words.begin(), words.end(), "/") << endl;

  cout << "[demo] CutForSearch" << endl;
  jb.CutForSearch(s, words);
  cout << limonp::Join(words.begin(), words.end(), "/") << endl;

  cout << "[demo] Insert User Word" << endl;
  jb.Cut("男默女泪", words);
  cout << limonp::Join(words.begin(), words.end(), "/") << endl;
  jb.InsertUserWord("男默女泪");
  jb.Cut("男默女泪", words);
  cout << limonp::Join(words.begin(), words.end(), "/") << endl;

  cout << "[demo] Locate Words" << endl;
  vector<cppjb::Jb::LocWord> loc_words;
  jb.Cut("南京市长江大桥", words, true);
  jb.Locate(words, loc_words);
  for (size_t i = 0; i < loc_words.size(); i++) {
    cout << loc_words[i].word 
      << ", " << loc_words[i].begin
      << ", " << loc_words[i].end
      << endl;
  }

  cout << "[demo] TAGGING" << endl;
  vector<pair<string, string> > tagres;

  string input = s;
	string output;
	_gbk_to_utf8(input, output);
	jb.Tag(output, tagres);
	/*for(int i = 0; i < tagres.size(); i++)
	{
		cerr << tagres[i].second << " ";
	}*/
  cout << s << endl;
  cout << tagres << endl;;

  cppjb::KeywordExtractor extractor(jb,
        IDF_PATH,
        STOP_WORD_PATH);
  cout << "[demo] KEYWORD" << endl;
  const size_t topk = 5;
  vector<pair<string, double> > keywordres;
  extractor.Extract(s, keywordres, topk);
  cout << s << endl;
  cout << keywordres << endl;
  }
  return EXIT_SUCCESS;
}
