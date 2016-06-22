#ifndef CPPJIEAB_JB_H
#define CPPJIEAB_JB_H

#include "QuerySegment.hpp"
#include "PosTagger.hpp"
#include "LevelSegment.hpp"
#include "DictTrie.hpp"

namespace cppjb {

class Jb {
 public:
  Jb(const std::string& dict_path, const std::string& model_path, const std::string& user_dict_path) 
    : dict_trie_(dict_path, user_dict_path),
      model_(model_path),
      mp_seg_(&dict_trie_),
      hmm_seg_(&model_),
      mix_seg_(&dict_trie_, &model_),
      full_seg_(&dict_trie_),
      query_seg_(&dict_trie_, &model_),
      level_seg_(&dict_trie_),
      pos_tagger_(&dict_trie_, &model_) {
  }
  
	 Jb()
	 {
	 }
  ~Jb() {
  }


  void loadDic(const std::string& dict_path, const std::string& model_path, const std::string& user_dict_path) ;

  struct LocWord {
    std::string word;
    size_t begin;
    size_t end;
  }; // struct LocWord

  void Cut(const std::string& sentence, std::vector<std::string>& words, bool hmm = true) const {
    mix_seg_.Cut(sentence, words, hmm);
  }
  void CutAll(const std::string& sentence, std::vector<std::string>& words) const {
    full_seg_.Cut(sentence, words);
  }
  void CutForSearch(const std::string& sentence, std::vector<std::string>& words, bool hmm = true) const {
    query_seg_.Cut(sentence, words, hmm);
  }
  void CutHMM(const std::string& sentence, std::vector<std::string>& words) const {
    hmm_seg_.Cut(sentence, words);
  }
  void CutLevel(const std::string& sentence, std::vector<std::string>& words) const {
    level_seg_.Cut(sentence, words);
  }
  void CutLevel(const std::string& sentence, std::vector<std::pair<std::string, size_t> >& words) const {
    level_seg_.Cut(sentence, words);
  }
  void CutSmall(const std::string& sentence, std::vector<std::string>& words, size_t max_word_len) const {
    mp_seg_.Cut(sentence, words, max_word_len);
  }
  void Locate(const std::vector<std::string>& words, std::vector<LocWord>& loc_words) const {
    loc_words.resize(words.size());
    size_t begin = 0;
    for (size_t i = 0; i < words.size(); i++) {
      size_t len = TransCode::Decode(words[i]).size();
      loc_words[i].word = words[i];
      loc_words[i].begin = begin;
      loc_words[i].end = loc_words[i].begin + len;
      begin = loc_words[i].end;
    }
  }
  
  void Tag(const std::string& sentence, std::vector<std::pair<std::string, std::string> >& words) ;
 /* const {
    pos_tagger_.Tag(sentence, words);
  }
  */
  bool InsertUserWord(const std::string& word, const std::string& tag = UNKNOWN_TAG) {
    return dict_trie_.InsertUserWord(word, tag);
  }

  const DictTrie* GetDictTrie() const {
    return &dict_trie_;
  } 
  const HMMModel* GetHMMModel() const {
    return &model_;
  }
 
  void SetQuerySegmentThreshold(size_t len) {
    query_seg_.SetMaxWordLen(len);
  }
 private:
  DictTrie dict_trie_;
  HMMModel model_;
  
  // They share the same dict trie and model
  MPSegment mp_seg_;
  HMMSegment hmm_seg_;
  MixSegment mix_seg_;
  FullSegment full_seg_;
  QuerySegment query_seg_;
  LevelSegment level_seg_;
  
  PosTagger pos_tagger_;
  
}; // class Jb

} // namespace cppjb

#endif // CPPJIEAB_JB_H
