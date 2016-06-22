#ifndef CPPJB_LEVELSEGMENT_H
#define CPPJB_LEVELSEGMENT_H

#include "MPSegment.hpp"

namespace cppjb {

class LevelSegment: public SegmentBase{
 public:
  LevelSegment(const std::string& dictPath, 
               const std::string& userDictPath = "")
    : mpSeg_(dictPath, userDictPath) {
  }
 
  LevelSegment(const DictTrie* dictTrie) 
    : mpSeg_(dictTrie) {
  }
  
  LevelSegment()
  {
  }
  ~LevelSegment() {
  }
  void loadDic(const DictTrie* dictTrie) 
  {
	  mpSeg_.loadDic(dictTrie);
  }

  void Cut(Unicode::const_iterator begin,
        Unicode::const_iterator end, 
        std::vector<std::pair<Unicode, size_t> >& res) const {
    res.clear();
    std::vector<Unicode> words;
    std::vector<Unicode> smallerWords;
    words.reserve(end - begin);
    mpSeg_.Cut(begin, end, words);
    smallerWords.reserve(words.size());
    res.reserve(words.size());

    size_t level = 0;
    while (!words.empty()) {
      smallerWords.clear();
      for (size_t i = 0; i < words.size(); i++) {
        if (words[i].size() >= 3) {
          size_t len = words[i].size() - 1;
          mpSeg_.Cut(words[i].begin(), words[i].end(), smallerWords, len); // buffer.push_back without clear 
        }
        if (words[i].size() > 1) {
          res.push_back(std::pair<Unicode, size_t>(words[i], level));
        }
      }

      words.swap(smallerWords);
      level++;
    }
  }

  void Cut(const std::string& sentence, 
        std::vector<std::pair<std::string, size_t> >& words) const {
    words.clear();
    Unicode unicode;
    TransCode::Decode(sentence, unicode);
    std::vector<std::pair<Unicode, size_t> > unicodeWords;
    Cut(unicode.begin(), unicode.end(), unicodeWords);
    words.resize(unicodeWords.size());
    for (size_t i = 0; i < words.size(); i++) {
      TransCode::Encode(unicodeWords[i].first, words[i].first);
      words[i].second = unicodeWords[i].second;
    }
  }

  bool Cut(const std::string& sentence, 
        std::vector<std::string>& res) const {
    std::vector<std::pair<std::string, size_t> > words;
    Cut(sentence, words);
    res.clear();
    res.reserve(words.size());
    for (size_t i = 0; i < words.size(); i++) {
      res.push_back(words[i].first);
    }
    return true;
  }

 private:
  MPSegment mpSeg_;
}; // class LevelSegment

} // namespace cppjb

#endif // CPPJB_LEVELSEGMENT_H
