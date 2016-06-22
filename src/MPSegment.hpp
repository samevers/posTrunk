#ifndef CPPJB_MPSEGMENT_H
#define CPPJB_MPSEGMENT_H

#include <algorithm>
#include <set>
#include <cassert>
#include "limonp/Logging.hpp"
#include "DictTrie.hpp"
#include "SegmentBase.hpp"

namespace cppjb {

class MPSegment: public SegmentBase {
 public:
  MPSegment(const std::string& dictPath, const std::string& userDictPath = "") {
    dictTrie_ = new DictTrie(dictPath, userDictPath);
    isNeedDestroy_ = true;
  }
  MPSegment(const DictTrie* dictTrie)
    : dictTrie_(dictTrie), isNeedDestroy_(false) {
    assert(dictTrie_);
  }
  MPSegment()
  {
  }
  void loadDic(const DictTrie* dictTrie)
  {
    dictTrie_ = dictTrie;
	isNeedDestroy_ = false;
    assert(dictTrie_);
  }

  ~MPSegment() {
    if (isNeedDestroy_) {
      delete dictTrie_;
    }
  }

  void Cut(const std::string& sentence, 
        std::vector<std::string>& words, 
        size_t max_word_len = MAX_WORD_LENGTH) const {
    PreFilter pre_filter(symbols_, sentence);
    PreFilter::Range range;
    std::vector<Unicode> uwords;
    uwords.reserve(sentence.size());
    while (pre_filter.HasNext()) {
      range = pre_filter.Next();
      Cut(range.begin, range.end, uwords, max_word_len);
    }
    TransCode::Encode(uwords, words);
  }
  void Cut(Unicode::const_iterator begin,
           Unicode::const_iterator end,
           std::vector<Unicode>& words,
           size_t max_word_len = MAX_WORD_LENGTH) const {
    std::vector<Dag> dags;
    dictTrie_->Find(begin, 
          end, 
          dags,
          max_word_len);
    CalcDP(dags);
    CutByDag(dags, words);
  }

  const DictTrie* GetDictTrie() const {
    return dictTrie_;
  }

  bool IsUserDictSingleChineseWord(const Rune& value) const {
    return dictTrie_->IsUserDictSingleChineseWord(value);
  }
 private:
  void CalcDP(std::vector<Dag>& dags) const {
    size_t nextPos;
    const DictUnit* p;
    double val;

    for (std::vector<Dag>::reverse_iterator rit = dags.rbegin(); rit != dags.rend(); rit++) {
      rit->pInfo = NULL;
      rit->weight = MIN_DOUBLE;
      assert(!rit->nexts.empty());
      for (limonp::LocalVector<std::pair<size_t, const DictUnit*> >::const_iterator it = rit->nexts.begin(); it != rit->nexts.end(); it++) {
        nextPos = it->first;
        p = it->second;
        val = 0.0;
        if (nextPos + 1 < dags.size()) {
          val += dags[nextPos + 1].weight;
        }

        if (p) {
          val += p->weight;
        } else {
          val += dictTrie_->GetMinWeight();
        }
        if (val > rit->weight) {
          rit->pInfo = p;
          rit->weight = val;
        }
      }
    }
  }
  void CutByDag(const std::vector<Dag>& dags, 
        std::vector<Unicode>& words) const {
    size_t i = 0;
    while (i < dags.size()) {
      const DictUnit* p = dags[i].pInfo;
      if (p) {
        words.push_back(p->word);
        i += p->word.size();
      } else { //single chinese word
        words.push_back(Unicode(1, dags[i].rune));
        i++;
      }
    }
  }

  const DictTrie* dictTrie_;
  bool isNeedDestroy_;
}; // class MPSegment

} // namespace cppjb

#endif
