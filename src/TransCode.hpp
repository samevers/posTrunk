/************************************
 * file enc : utf-8
 * author   : wuyanyi09@gmail.com
 ************************************/
#ifndef CPPJB_TRANSCODE_H
#define CPPJB_TRANSCODE_H


#include "limonp/StringUtil.hpp"
#include "limonp/LocalVector.hpp"

namespace cppjb {

//using namespace limonp;

typedef uint16_t Rune;
typedef limonp::LocalVector<Rune> Unicode;

namespace TransCode {
inline bool Decode(const std::string& str, Unicode& res) {
#ifdef CPPJB_GBK
  return gbkTrans(str, res);
#else
  return Utf8ToUnicode(str, res);
#endif
}

inline void Encode(Unicode::const_iterator begin, Unicode::const_iterator end, std::string& res) {
#ifdef CPPJB_GBK
	gbkTrans(begin, end, res);
#else
  limonp::UnicodeToUtf8(begin, end, res);
#endif
}

inline void Encode(const Unicode& uni, std::string& res) {
  Encode(uni.begin(), uni.end(), res);
}

// compiler is expected to optimized this function to avoid return value copy
inline std::string Encode(Unicode::const_iterator begin, Unicode::const_iterator end) {
  std::string res;
  res.reserve(end - begin);
  Encode(begin, end, res);
  return res;
}

inline std::string Encode(const Unicode& unicode) {
  return Encode(unicode.begin(), unicode.end());
}

// compiler is expected to optimized this function to avoid return value copy
inline Unicode Decode(const std::string& str) {
  Unicode unicode;
  unicode.reserve(str.size());
  Decode(str, unicode);
  return unicode;
}

inline void Encode(const std::vector<Unicode>& input, std::vector<std::string>& output) {
  output.resize(input.size());
  for (size_t i = 0; i < output.size(); i++) {
    Encode(input[i], output[i]);
  }
}

} // namespace TransCode
} // namespace cppjb

#endif
