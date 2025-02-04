#include "data.h"
#include "implot.h"
#include <cmath>

void DrawableData::draw() const {
//   ImPlotFlags flags = ImPlotFlags_NoInputs;
//   if (ImPlot::BeginPlot("Graph", ImVec2(-1, -1), flags)) {
//     ImPlot::SetupAxes("Wavelength (nm)", "Light Intensity");
    ImPlot::PlotLine("Live data", xs.data(), ys.data(), xs.size());
    // ImPlot::EndPlot();
//   }
}

RawData RandomDataGenerator::generate() {
  RawData rawData;
  std::generate(std::begin(rawData.data), std::end(rawData.data),
                [this]() { return randomValue(); });
  static float offset = 0.0;
  offset += 0.01;
  for (size_t i = 0; i < RawData::N_SAMPLES; i++) {
    rawData.data[i] += std::sin(i / 10.0 + offset) * 80 + 80;
  }
  return rawData;
}

uint8_t RandomDataGenerator::randomValue() { return dist(rng); }

RandomDataGenerator::RandomDataGenerator()
    : rng(std::random_device{}()), dist(0, 80) {}

DrawableData::DrawableData(const RawData &raw_data) {
  for (size_t i = 0; i < RawData::N_SAMPLES; i++) {
    float y = static_cast<float>(raw_data.data[i]) / 255.0F;
    float bandwidth = raw_data.max_wavelength - raw_data.min_wavelength;
    float x =
        raw_data.min_wavelength + bandwidth * i / (RawData::N_SAMPLES - 1);

    xs.push_back(x);
    ys.push_back(y);
  }
}

DrawableData::DrawableData(const NormalizedData &normalized_data) {
  for (size_t i = 0; i < RawData::N_SAMPLES; i++) {
    float y = normalized_data.data[i];
    float bandwidth = RawData::max_wavelength - RawData::min_wavelength;
    float x =
        RawData::min_wavelength + bandwidth * i / (RawData::N_SAMPLES - 1);
    xs.push_back(x);
    ys.push_back(y);
  }

}

NormalizedData::NormalizedData(const RawData &raw_data) {
  for (size_t i = 0; i < RawData::N_SAMPLES; i++) {
    data[i] = static_cast<float>(raw_data.data[i]) / 255.0;
  }
}
