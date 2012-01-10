// IMM UB RAS (c) 2012
// author : Aleksander A. Popov
// encoding : utf8
// кодировка : utf8

#ifndef TRM_CONVERTER_DATA_H_
#define TRM_CONVERTER_DATA_H_

#include <vector>

class Data {
public:
  /**
   */
  Data();

  /**
   */
  ~Data();

  /**
   */
  void load(const char* filename);

  /**
   */
  void save(const char* filename);

  /**
   */
  bool isLoaded() const;

protected:
  bool is_loaded_;
  std::vector<double> ch0_;
  std::vector<double> ch1_;
  std::vector<double> conv_ch0_;
  std::vector<double> conv_ch1_;
  int freq_;
};

#endif // TRM_CONVERTER_DATA_H_
