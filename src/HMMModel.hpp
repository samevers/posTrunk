#ifndef CPPJB_HMMMODEL_H
#define CPPJB_HMMMODEL_H

#include "limonp/StringUtil.hpp"

namespace cppjb {

//using namespace limonp;
typedef std::unordered_map<uint16_t, double> EmitProbMap;

struct HMMModel {
  /*
   * STATUS:
   * 0: HMMModel::B, 1: HMMModel::E, 2: HMMModel::M, 3:HMMModel::S
   * */
  enum {B = 0, E = 1, M = 2, S = 3, STATUS_SUM = 4};

  
  HMMModel(const std::string& modelPath) {
    memset(startProb, 0, sizeof(startProb));
    memset(transProb, 0, sizeof(transProb));
    statMap[0] = 'B';
    statMap[1] = 'E';
    statMap[2] = 'M';
    statMap[3] = 'S';
    emitProbVec.push_back(&emitProbB);
    emitProbVec.push_back(&emitProbE);
    emitProbVec.push_back(&emitProbM);
    emitProbVec.push_back(&emitProbS);
    LoadModel(modelPath);
  }
  
 HMMModel()
 {
}
  ~HMMModel() {
  }
void loadDic(const std::string& modelPath) {
    memset(startProb, 0, sizeof(startProb));
    memset(transProb, 0, sizeof(transProb));
    statMap[0] = 'B';
    statMap[1] = 'E';
    statMap[2] = 'M';
    statMap[3] = 'S';
    emitProbVec.push_back(&emitProbB);
    emitProbVec.push_back(&emitProbE);
    emitProbVec.push_back(&emitProbM);
    emitProbVec.push_back(&emitProbS);
    LoadModel(modelPath);
  }

  void LoadModel(const std::string& filePath) {
	  std::ifstream ifile(filePath.c_str());
	  if(!ifile)
	  {
		  std::cerr << "[ERROR] Fail to open file: "<< filePath << std::endl;
		  return;
	  }
    std::string line;
    std::vector<std::string> tmp;
    std::vector<std::string> tmp2;
    //Load startProb
	if(!GetLine(ifile, line))
	{
		  std::cerr << "[ERROR] Fail to open file: "<< filePath << std::endl;
		  return;
	}

	limonp::Split(line, tmp, " ");
	if(tmp.size() != STATUS_SUM){
		  std::cerr << "[ERROR] Fail to open file: "<< filePath << std::endl;
		  return;
	}

    for (size_t j = 0; j< tmp.size(); j++) {
      startProb[j] = atof(tmp[j].c_str());
    }

    //Load transProb
    for (size_t i = 0; i < STATUS_SUM; i++) {
	  if(!GetLine(ifile, line))
	  {
		  std::cerr << "[ERROR] Fail to open file: "<< filePath << std::endl;
		  break;
	  }
      limonp::Split(line, tmp, " ");
	  if(tmp.size() != STATUS_SUM){
		  std::cerr << "[ERROR] Fail to open file: "<< filePath << std::endl;
		  break;
	  }
      for (size_t j =0; j < STATUS_SUM; j++) {
		std::cerr << "tmp["<<j<<"] = " << tmp[j] << std::endl;
        transProb[i][j] = atof(tmp[j].c_str());
      }
    }

    //Load emitProbB
	if(!GetLine(ifile, line))
	{
		return;
	}
    if(!LoadEmitProb(line, emitProbB))
	{
		return;
	}

    //Load emitProbE
    if(!GetLine(ifile, line))
	{
		return;
	}
	if(!LoadEmitProb(line, emitProbE))
	{
		return;
	}

    //Load emitProbM
     if(!GetLine(ifile, line))
	{
		return;
	}
	if(!LoadEmitProb(line, emitProbM))
	{
		return;
	}

    //Load emitProbS
     if(!GetLine(ifile, line))
	{
		return;
	}
    if(!LoadEmitProb(line, emitProbS))
	{
		return;
	}
  }
  double GetEmitProb(const EmitProbMap* ptMp, uint16_t key, 
        double defVal)const {
    EmitProbMap::const_iterator cit = ptMp->find(key);
    if (cit == ptMp->end()) {
      return defVal;
    }
    return cit->second;
  }
  bool GetLine(std::ifstream& ifile, std::string& line) {
    while (getline(ifile, line)) {
		limonp::Trim_(line);
      if (line.empty()) {
        continue;
      }
      if (limonp::StartsWith(line, "#")) {
        continue;
      }
      return true;
    }
    return false;
  }
  bool LoadEmitProb(const std::string& line, EmitProbMap& mp) {
    if (line.empty()) {
      return false;
    }
    std::vector<std::string> tmp, tmp2;
    Unicode unicode;
	limonp::Split(line, tmp, ",");
    for (size_t i = 0; i < tmp.size(); i++) {
		limonp::Split(tmp[i], tmp2, ":");
      if (2 != tmp2.size()) {
//        limonp::LOG(ERROR) << "emitProb illegal.";
        return false;
      }
      if (!TransCode::Decode(tmp2[0], unicode) || unicode.size() != 1) {
//        limonp::LOG(ERROR) << "TransCode failed.";
        return false;
      }
      mp[unicode[0]] = atof(tmp2[1].c_str());
    }
    return true;
  }

  char statMap[STATUS_SUM];
  double startProb[STATUS_SUM];
  double transProb[STATUS_SUM][STATUS_SUM];
  EmitProbMap emitProbB;
  EmitProbMap emitProbE;
  EmitProbMap emitProbM;
  EmitProbMap emitProbS;
  std::vector<EmitProbMap* > emitProbVec;
}; // struct HMMModel

} // namespace cppjb

#endif
