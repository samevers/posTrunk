#ifndef CPPJB_DICT_TRIE_HPP
#define CPPJB_DICT_TRIE_HPP

#include <iostream>
#include <fstream>
#include <map>
#include <cstring>
#include <stdint.h>
#include <cmath>
#include <limits>
#include "limonp/StringUtil.hpp"
#include "limonp/Logging.hpp"
#include "TransCode.hpp"
#include "Trie.hpp"

namespace cppjb {

//using namespace limonp;

const double MIN_DOUBLE = -3.14e+100;
const double MAX_DOUBLE = 3.14e+100;
const size_t DICT_COLUMN_NUM = 3;
const char* const UNKNOWN_TAG = "";

class DictTrie {
 public:
  enum UserWordWeightOption {
    WordWeightMin,
    WordWeightMedian,
    WordWeightMax,
  }; // enum UserWordWeightOption
  DictTrie()
  {
  }
  DictTrie(const std::string& dict_path, const std::string& user_dict_paths = "", UserWordWeightOption user_word_weight_opt = WordWeightMedian) {
    Init(dict_path, user_dict_paths, user_word_weight_opt);
  }

  ~DictTrie() {
    delete trie_;
  }
  void loadDic(const std::string& dict_path, const std::string& user_dict_paths = "", UserWordWeightOption user_word_weight_opt = WordWeightMedian) {
    Init(dict_path, user_dict_paths, user_word_weight_opt);
  }

  bool InsertUserWord(const std::string& word, const std::string& tag = UNKNOWN_TAG) {
    DictUnit node_info;
    if (!MakeNodeInfo(node_info, word, user_word_default_weight_, tag)) {
      return false;
    }
    active_node_infos_.push_back(node_info);
    trie_->InsertNode(node_info.word, &active_node_infos_.back());
    return true;
  }

  const DictUnit* Find(Unicode::const_iterator begin, Unicode::const_iterator end) const {
    return trie_->Find(begin, end);
  }

  void Find(Unicode::const_iterator begin, 
        Unicode::const_iterator end, 
        std::vector<struct Dag>&res,
        size_t max_word_len = MAX_WORD_LENGTH) const {
    trie_->Find(begin, end, res, max_word_len);
  }

  bool IsUserDictSingleChineseWord(const Rune& word) const {
    return IsIn(user_dict_single_chinese_word_, word);
  }

  double GetMinWeight() const {
    return min_weight_;
  }

 private:
  void Init(const std::string& dict_path, const std::string& user_dict_paths, UserWordWeightOption user_word_weight_opt) {
    LoadDict(dict_path);
    CalculateWeight(static_node_infos_);
    SetStaticWordWeights(user_word_weight_opt);

    if (user_dict_paths.size()) {
      LoadUserDict(user_dict_paths);
    }
    Shrink(static_node_infos_);
    CreateTrie(static_node_infos_);
  }
  
  void CreateTrie(const std::vector<DictUnit>& dictUnits) {
    assert(dictUnits.size());
    std::vector<Unicode> words;
    std::vector<const DictUnit*> valuePointers;
    for (size_t i = 0 ; i < dictUnits.size(); i ++) {
      words.push_back(dictUnits[i].word);
      valuePointers.push_back(&dictUnits[i]);
    }

    trie_ = new Trie(words, valuePointers);
  }

  void LoadUserDict(const std::string& filePaths) {
    std::vector<std::string> files = limonp::Split(filePaths, "|;");
    size_t lineno = 0;
    for (size_t i = 0; i < files.size(); i++) {
		std::ifstream ifs(files[i].c_str());
		if(!ifs)
		{
			std::cerr << "[ERROR] Fail to open file: " << files[i] << std::endl;
			return;
		}
      std::string line;
      DictUnit node_info;
      std::vector<std::string> buf;
      for (; getline(ifs, line); lineno++) {
        if (line.size() == 0) {
          continue;
        }
        buf.clear();
		limonp::Split(line, buf, " ");
        DictUnit node_info;
        MakeNodeInfo(node_info, 
              buf[0], 
              user_word_default_weight_,
              (buf.size() == 2 ? buf[1] : UNKNOWN_TAG));
        static_node_infos_.push_back(node_info);
        if (node_info.word.size() == 1) {
          user_dict_single_chinese_word_.insert(node_info.word[0]);
        }
      }
    }
    //limonp::LOG(INFO) << "load userdicts " << filePaths << ", lines: " << lineno;
  }

  bool MakeNodeInfo(DictUnit& node_info,
        const std::string& word, 
        double weight, 
        const std::string& tag) {
    if (!TransCode::Decode(word, node_info.word)) {
//      limonp::LOG(ERROR) << "Decode " << word << " failed.";
      return false;
    }
    node_info.weight = weight;
    node_info.tag = tag;
    return true;
  }

  void LoadDict(const std::string& filePath) {
	  std::ifstream ifs(filePath.c_str());
	  if(!ifs)
	  {
		  std::cerr << "[ERROR] Fail to open file:" << filePath << std::endl;
		  return;
	  }
    std::string line;
    std::vector<std::string> buf;

    DictUnit node_info;
    for (size_t lineno = 0; getline(ifs, line); lineno++) {
		limonp::Split(line, buf, " ");
		if(buf.size() != DICT_COLUMN_NUM)
		{
			std::cerr << "[ERROR] bus.size != DICT_COLUMN_NUM "  << std::endl;
			return;
		}
      MakeNodeInfo(node_info, 
            buf[0], 
            atof(buf[1].c_str()), 
            buf[2]);
      static_node_infos_.push_back(node_info);
    }
  }

  static bool WeightCompare(const DictUnit& lhs, const DictUnit& rhs) {
    return lhs.weight < rhs.weight;
  }

  void SetStaticWordWeights(UserWordWeightOption option) {
	if(static_node_infos_.empty())
	{
		std::cerr << "[ERROR] static_node_infos_ is empty." << std::endl;
		return;
	}
    std::vector<DictUnit> x = static_node_infos_;
    sort(x.begin(), x.end(), WeightCompare);
    min_weight_ = x[0].weight;
    max_weight_ = x[x.size() - 1].weight;
    median_weight_ = x[x.size() / 2].weight;
    switch (option) {
     case WordWeightMin:
       user_word_default_weight_ = min_weight_;
       break;
     case WordWeightMedian:
       user_word_default_weight_ = median_weight_;
       break;
     default:
       user_word_default_weight_ = max_weight_;
       break;
    }
  }

  void CalculateWeight(std::vector<DictUnit>& node_infos) const {
    double sum = 0.0;
    for (size_t i = 0; i < node_infos.size(); i++) {
      sum += node_infos[i].weight;
    }
    assert(sum);
    for (size_t i = 0; i < node_infos.size(); i++) {
      DictUnit& node_info = node_infos[i];
      assert(node_info.weight);
      node_info.weight = log(double(node_info.weight)/double(sum));
    }
  }

  void Shrink(std::vector<DictUnit>& units) const {
    std::vector<DictUnit>(units.begin(), units.end()).swap(units);
  }

  std::vector<DictUnit> static_node_infos_;
  std::deque<DictUnit> active_node_infos_; // must not be std::vector
  Trie * trie_;

  double min_weight_;
  double max_weight_;
  double median_weight_;
  double user_word_default_weight_;
  std::unordered_set<Rune> user_dict_single_chinese_word_;
};
}

#endif
