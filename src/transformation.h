#pragma once

#include "data.h"
#include <functional>
#include <string>

inline NormalizedData avg(const std::vector<NormalizedData> &input) {
  NormalizedData ret{};

  size_t n_samples{0};
  for (const auto &shot : input) {
    n_samples++;
    for (size_t i = 0; i < RawData::N_SAMPLES; i++) {
      ret.data[i] += shot.data[i];
    }
  }

  for (size_t i = 0; i < RawData::N_SAMPLES; i++) {
    ret.data[i] /= n_samples;
  }

  return ret;
}

inline NormalizedData live_data(const std::vector<NormalizedData> &input) {
    if (input.empty())
        return {};
    return input.back();
}

struct Transformation {
  std::string name;
  std::function<NormalizedData(const std::vector<NormalizedData> &)> func;
  bool enabled;
};
