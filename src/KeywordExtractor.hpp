#ifndef CPPJB_KEYWORD_EXTRACTOR_H
#define CPPJB_KEYWORD_EXTRACTOR_H

#include <cmath>
#include <set>
#include "Jb.hpp"

namespace cppjb {
//using namespace limonp;

/*utf8*/
class KeywordExtractor {
 public:
  KeywordExtractor(const std::string& dictPath, 
        const std::string& hmmFilePath, 
        const std::string& idfPath, 
        const std::string& stopWordPath, 
        const std::string& userDict = "") 
    : segment_(dictPath, hmmFilePath, userDict) {
    LoadIdfDict(idfPath);
    LoadStopWordDict(stopWordPath);
  }
  KeywordExtractor(const DictTrie* dictTrie, 
        const HMMModel* model,
        const std::string& idfPath, 
        const std::string& stopWordPath) 
    : segment_(dictTrie, model) {
    LoadIdfDict(idfPath);
    LoadStopWordDict(stopWordPath);
  }
  KeywordExtractor(const Jb& jb, 
        const std::string& idfPath, 
        const std::string& stopWordPath) 
    : segment_(jb.GetDictTrie(), jb.GetHMMModel()) {
    LoadIdfDict(idfPath);
    LoadStopWordDict(stopWordPath);
  }
  ~KeywordExtractor() {
  }

  bool Extract(const std::string& sentence, std::vector<std::string>& keywords, size_t topN) const {
    std::vector<std::pair<std::string, double> > topWords;
    if (!Extract(sentence, topWords, topN)) {
      return false;
    }
    for (size_t i = 0; i < topWords.size(); i++) {
      keywords.push_back(topWords[i].first);
    }
    return true;
  }

  bool Extract(const std::string& sentence, std::vector<std::pair<std::string, double> >& keywords, size_t topN) const {
    std::vector<std::string> words;
    segment_.Cut(sentence, words);

    std::map<std::string, double> wordmap;
    for (std::vector<std::string>::iterator iter = words.begin(); iter != words.end(); iter++) {
      if (IsSingleWord(*iter)) {
        continue;
      }
      wordmap[*iter] += 1.0;
    }

    for (std::map<std::string, double>::iterator itr = wordmap.begin(); itr != wordmap.end(); ) {
      if (stopWords_.end() != stopWords_.find(itr->first)) {
        wordmap.erase(itr++);
        continue;
      }

	  std::unordered_map<std::string, double>::const_iterator cit = idfMap_.find(itr->first);
      if (cit != idfMap_.end()) {
        itr->second *= cit->second;
      } else {
        itr->second *= idfAverage_;
      }
      itr ++;
    }

    keywords.clear();
    std::copy(wordmap.begin(), wordmap.end(), std::inserter(keywords, keywords.begin()));
    topN = std::min(topN, keywords.size());
    partial_sort(keywords.begin(), keywords.begin() + topN, keywords.end(), Compare);
    keywords.resize(topN);
    return true;
  }
 private:
  void LoadIdfDict(const std::string& idfPath) {
	  std::ifstream ifs(idfPath.c_str());
//	  limonp::CHECK_(ifs.is_open()) << "open " << idfPath << " failed";
    std::string line ;
    std::vector<std::string> buf;
    double idf = 0.0;
    double idfSum = 0.0;
    size_t lineno = 0;
    for (; getline(ifs, line); lineno++) {
      buf.clear();
      if (line.empty()) {
    //    LOG(ERROR) << "lineno: " << lineno << " empty. skipped.";
        continue;
      }
	  limonp::Split(line, buf, " ");
      if (buf.size() != 2) {
     //   LOG(ERROR) << "line: " << line << ", lineno: " << lineno << " empty. skipped.";
        continue;
      }
      idf = atof(buf[1].c_str());
      idfMap_[buf[0]] = idf;
      idfSum += idf;

    }

    assert(lineno);
    idfAverage_ = idfSum / lineno;
    assert(idfAverage_ > 0.0);
  }
  void LoadStopWordDict(const std::string& filePath) {
	  std::ifstream ifs(filePath.c_str());
//    CHECK(ifs.is_open()) << "open " << filePath << " failed";
    std::string line ;
    while (getline(ifs, line)) {
      stopWords_.insert(line);
    }
    assert(stopWords_.size());
  }

  bool IsSingleWord(const std::string& str) const {
    Unicode unicode;
    TransCode::Decode(str, unicode);
    if (unicode.size() == 1)
      return true;
    return false;
  }

  static bool Compare(const std::pair<std::string, double>& lhs, const std::pair<std::string, double>& rhs) {
    return lhs.second > rhs.second;
  }

  MixSegment segment_;
  std::unordered_map<std::string, double> idfMap_;
  double idfAverage_;

  std::unordered_set<std::string> stopWords_;
}; // class Jb
} // namespace cppjb

#endif


