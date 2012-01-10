// Copyright (C) 2012, IMM UB RAS
// Copyright (C) 2012, Aleksander A. Popov <x100@yandex.ru>
// encoding : utf8
// кодировка : utf8

#include "data.h"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

namespace {

struct SignalInfo {
  int m_Frequency;
  // y = A*(x + B)
  float m_A;
  float m_B;
  int m_CanalAmount;
  int m_ByteToCanal;
  int m_CanalSize;
};


bool GetEndOfHeader(const char* pFileData,
                    int nDataSize,
                    int* pIndexOfEnd) {
  if (! pFileData) return false;
  if (! pIndexOfEnd) return false;

  const char* HeaderEndStr = "END HEADER\n";
  const char* CurSymbol = HeaderEndStr;

  int nEndHeader = 0;
  for (; nEndHeader < nDataSize; ++nEndHeader) {
    if (pFileData[nEndHeader] == *CurSymbol) {
      ++CurSymbol;
      if(0 == *CurSymbol) {
        *pIndexOfEnd = nEndHeader;
        return true;
      }
    } else {
      CurSymbol = HeaderEndStr;
    }
  }
  return false;
}

bool ReadTrm(const char* lpFileName,
             vector<double>& ch0,
             vector<double>& ch1,
             int& freq)
{
  bool res = false;
  FILE* fd = fopen(lpFileName, "rb");
  int ByteSize;
  fseek(fd, 0, SEEK_END);
  ByteSize = ftell(fd);
  fseek(fd, 0, SEEK_SET);
  char* WholeFile = new char[ByteSize];
  fread(WholeFile, ByteSize, 1, fd);
  fclose(fd);

  int nEndHeader = 0;
  bool bFoundHeader = GetEndOfHeader(WholeFile, ByteSize, &nEndHeader);

  SignalInfo si = {0};

  if (bFoundHeader) {
    nEndHeader++;

    memcpy(&si, WholeFile + nEndHeader, sizeof(si));

    ByteSize -= nEndHeader + sizeof(si);

    if (si.m_CanalSize > ByteSize / si.m_ByteToCanal * si.m_CanalAmount) {
      si.m_CanalSize = ByteSize / si.m_ByteToCanal * si.m_CanalAmount;
    }

    int k;
    short* ReadBuf = (short*) (WholeFile + nEndHeader + sizeof(si));

    ch0.resize(si.m_CanalSize);
    ch1.resize(si.m_CanalSize);
    for (k = 0; k < si.m_CanalSize; k++) {
      ch0[k] = si.m_A * (ReadBuf[ k*2 + 0 ] + si.m_B);
      ch1[k] = si.m_A * (ReadBuf[ k*2 + 1 ] + si.m_B);
    }
    freq = si.m_Frequency;
    res = true;
  }

  delete [] WholeFile;
  return res;
}

} // namespace

Data::Data() {
  is_loaded_ = false;
  freq_ = 1;
}

Data::~Data() {
  ;
}

void Data::load(const char* filename) {
  bool res = ReadTrm(filename, ch0_, ch1_, freq_);
  if (! res) {
    is_loaded_ = false;
    return;
  }
  // :TODO: average signals
  conv_ch0_ = ch0_;
  conv_ch1_ = ch1_;
  is_loaded_ = (conv_ch0_.size() && conv_ch1_.size());
}

void Data::save(const char* filename) {
  if (! is_loaded_) return;
  ofstream file;
  file.open(filename);
  file << "frequency = " << freq_ << " Hz\n\n";
  int s0 = conv_ch0_.size();
  int s1 = conv_ch1_.size();
  int s = (s0 < s1 ? s0 : s1);
  char buf[256];
  for (int i = 0; i < s; ++i) {
    sprintf(buf, "%f", conv_ch0_[i]);
    file << buf << "\t";
    sprintf(buf, "%f", conv_ch1_[i]);
    file << buf << "\n";
  }
  file.close();
}

bool Data::isLoaded() const {
  return is_loaded_;
}
