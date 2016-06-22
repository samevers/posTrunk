#ifndef CPPJB_POS_TAGGING_H
#define CPPJB_POS_TAGGING_H

#include "MixSegment.hpp"
#include "limonp/StringUtil.hpp"
#include "DictTrie.hpp"

namespace cppjb {
//using namespace limonp;

static const char* const POS_M = "m";
static const char* const POS_ENG_ = "eng";
static const char* const POS_X = "x";

class PosTagger {
 public:
  PosTagger(const std::string& dictPath,
    const std::string& hmmFilePath,
    const std::string& userDictPath = "")
    : segment_(dictPath, hmmFilePath, userDictPath) {
  }
  PosTagger(const DictTrie* dictTrie, const HMMModel* model) 
    : segment_(dictTrie, model) {
  }
  PosTagger(){
  }
  ~PosTagger() {
  }
void loadDic(const DictTrie* dictTrie, const HMMModel* model) {
     segment_.loadDic(dictTrie, model);
     dict = segment_.GetDictTrie();
  }

  bool Tag(const std::string& src, std::vector<std::pair<std::string, std::string> >& res) const {
    std::vector<std::string> CutRes;
    segment_.Cut(src, CutRes);

    const DictUnit *tmp = NULL;
    Unicode unico;
  	//const DictTrie * dict = segment_.GetDictTrie();
    assert(dict != NULL);
    for (std::vector<std::string>::iterator itr = CutRes.begin(); itr != CutRes.end(); ++itr) {
      if (!TransCode::Decode(*itr, unico)) {
//		  limonp::LOG(ERROR) << "Decode failed.";
        return false;
      }
      tmp = dict->Find(unico.begin(), unico.end());
      if (tmp == NULL || tmp->tag.empty()) {
        res.push_back(make_pair(*itr, SpecialRule(unico)));
      } else {
        res.push_back(make_pair(*itr, tmp->tag));
      }
    }
    return !res.empty();
  }
 public: 
 private:
  const char* SpecialRule(const Unicode& unicode) const {
    size_t m = 0;
    size_t eng = 0;
    for (size_t i = 0; i < unicode.size() && eng < unicode.size() / 2; i++) {
      if (unicode[i] < 0x80) {
        eng ++;
        if ('0' <= unicode[i] && unicode[i] <= '9') {
          m++;
        }
      }
    }
    // ascii char is not found
    if (eng == 0) {
      return POS_X;
    }
    // all the ascii is number char
    if (m == eng) {
      return POS_M;
    }
    // the ascii chars contain english letter
    return POS_ENG_;
  }

  MixSegment segment_;
  const DictTrie * dict;
}; // class PosTagger

} // namespace cppjb

#endif
