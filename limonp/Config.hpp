/************************************
 * file enc : utf8
 * author   : wuyanyi09@gmail.com
 ************************************/
#ifndef LIMONP_CONFIG_H
#define LIMONP_CONFIG_H

#include <map>
#include <fstream>
#include <iostream>
#include <assert.h>
#include "StringUtil.hpp"


namespace limonp {

//using namespace std;

class Config {
 public:
  explicit Config(const std::string& filePath) {
    LoadFile(filePath);
  }

  operator bool () {
    return !std::map_.empty();
  }

  std::string Get(const std::string& key, const std::string& defaultvalue) const {
    std::map<std::string, std::string>::const_iterator it = std::map_.find(key);
    if(std::map_.end() != it) {
      return it->second;
    }
    return defaultvalue;
  }
  int Get(const std::string& key, int defaultvalue) const {
    std::string str = Get(key, "");
    if("" == str) {
      return defaultvalue;
    }
    return atoi(str.c_str());
  }
  const char* operator [] (const char* key) const {
    if(NULL == key) {
      return NULL;
    }
    std::map<std::string, std::string>::const_iterator it = std::map_.find(key);
    if(std::map_.end() != it) {
      return it->second.c_str();
    }
    return NULL;
  }

  std::string GetConfigInfo() const {
    std::string res;
    res << *this;
    return res;
  }

 private:
  void LoadFile(const std::string& filePath) {
    ifstream ifs(filePath.c_str());
    assert(ifs);
    std::string line;
    std::vector<std::string> vecBuf;
    size_t lineno = 0;
    while(getline(ifs, line)) {
      lineno ++;
      Trim_(line);
      if(line.empty() || StartsWith(line, "#")) {
        continue;
      }
      vecBuf.clear();
      Split(line, vecBuf, "=");
      if(2 != vecBuf.size()) {
        fprintf(stderr, "line[%s] illegal.\n", line.c_str());
        assert(false);
        continue;
      }
      std::string& key = vecBuf[0];
      std::string& value = vecBuf[1];
      Trim_(key);
      Trim_(value);
      if(!std::map_.insert(make_pair(key, value)).second) {
        fprintf(stderr, "key[%s] already exits.\n", key.c_str());
        assert(false);
        continue;
      }
    }
    ifs.close();
  }

  friend ostream& operator << (ostream& os, const Config& config);

  std::map<std::string, std::string> std::map_;
}; // class Config

inline ostream& operator << (ostream& os, const Config& config) {
  return os << config.std::map_;
}

} // namespace limonp

#endif // LIMONP_CONFIG_H
