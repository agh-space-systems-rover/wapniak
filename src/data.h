#pragma once
#include <algorithm>
#include <cstdint>
#include <random>
#include <vector>

// This will be returned from the spectrograph
struct RawData {
  constexpr static size_t N_SAMPLES = 288;
  uint8_t data[N_SAMPLES];
  constexpr static uint16_t min_wavelength = 340; // nm
  constexpr static uint16_t max_wavelength = 850; // nm
};

struct NormalizedData {
  NormalizedData(const RawData &raw_data);
  NormalizedData() = default;

  float data[RawData::N_SAMPLES];
};

struct DrawableData {
  DrawableData() = default;
  DrawableData(const RawData &raw_data);
  DrawableData(const NormalizedData &normalized_data);
  std::vector<float> xs;
  std::vector<float> ys;

  void draw() const;
};

class RandomDataGenerator {
public:
  RandomDataGenerator();

  RawData generate();

private:
  std::mt19937 rng; // Mersenne Twister random number generator
  std::uniform_int_distribution<uint8_t> dist;

  uint8_t randomValue();
};

