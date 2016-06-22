#include "Jb.hpp"
using namespace std;

namespace cppjb {

void Jb::loadDic(const string& dict_path, const string& model_path, const string& user_dict_path) 
{
  	  dict_trie_.loadDic(dict_path, user_dict_path);
      model_.loadDic(model_path);
      mp_seg_.loadDic(&dict_trie_);
      hmm_seg_.loadDic(&model_);
      mix_seg_.loadDic(&dict_trie_, &model_);
      full_seg_.loadDic(&dict_trie_);
      query_seg_.loadDic(&dict_trie_, &model_);
      level_seg_.loadDic(&dict_trie_);
      pos_tagger_.loadDic(&dict_trie_, &model_) ;
}

void Jb::Tag(const string& sentence, vector<pair<string, string> >& words) 
{
    pos_tagger_.Tag(sentence, words);
}

} 

