#ifndef CPPJB_MIXSEGMENT_H
#define CPPJB_MIXSEGMENT_H

#include <cassert>
#include "MPSegment.hpp"
#include "HMMSegment.hpp"
#include "limonp/StringUtil.hpp"

namespace cppjb {
class MixSegment: public SegmentBase {
 public:
  MixSegment(const std::string& mpSegDict, const std::string& hmmSegDict, 
        const std::string& userDict = "") 
    : mpSeg_(mpSegDict, userDict), 
      hmmSeg_(hmmSegDict) {
  }
  MixSegment(const DictTrie* dictTrie, const HMMModel* model) 
    : mpSeg_(dictTrie), hmmSeg_(model) {
  }
  
  MixSegment(){
  }
  ~MixSegment() {
  }
  void loadDic(const DictTrie* dictTrie, const HMMModel* model) 
  {
	  mpSeg_ = dictTrie;
	  hmmSeg_ = model;
  }

  void Cut(const std::string& sentence, std::vector<std::string>& words, bool hmm = true) const {
    PreFilter pre_filter(symbols_, sentence);
    PreFilter::Range range;
    std::vector<Unicode> uwords;
    uwords.reserve(sentence.size());
    while (pre_filter.HasNext()) {
      range = pre_filter.Next();
      Cut(range.begin, range.end, uwords, hmm);
    }
    TransCode::Encode(uwords, words);
  }

  void Cut(Unicode::const_iterator begin, Unicode::const_iterator end, std::vector<Unicode>& res, bool hmm) const {
    if (!hmm) {
      mpSeg_.Cut(begin, end, res);
      return;
    }
    std::vector<Unicode> words;
    words.reserve(end - begin);
    mpSeg_.Cut(begin, end, words);

    std::vector<Unicode> hmmRes;
    hmmRes.reserve(end - begin);
    Unicode piece;
    piece.reserve(end - begin);
    for (size_t i = 0, j = 0; i < words.size(); i++) {
      //if mp Get a word, it's ok, put it into result
      if (1 != words[i].size() || (words[i].size() == 1 && mpSeg_.IsUserDictSingleChineseWord(words[i][0]))) {
        res.push_back(words[i]);
        continue;
      }

      // if mp Get a single one and it is not in userdict, collect it in sequence
      j = i;
      while (j < words.size() && 1 == words[j].size() && !mpSeg_.IsUserDictSingleChineseWord(words[j][0])) {
        piece.push_back(words[j][0]);
        j++;
      }

      // Cut the sequence with hmm
      hmmSeg_.Cut(piece.begin(), piece.end(), hmmRes);

      //put hmm result to result
      for (size_t k = 0; k < hmmRes.size(); k++) {
        res.push_back(hmmRes[k]);
      }

      //clear tmp vars
      piece.clear();
      hmmRes.clear();

      //let i jump over this piece
      i = j - 1;
    }
  }

  const DictTrie* GetDictTrie() const {
    return mpSeg_.GetDictTrie();
  }
 private:
  MPSegment mpSeg_;
  HMMSegment hmmSeg_;

}; // class MixSegment

} // namespace cppjb

#endif
