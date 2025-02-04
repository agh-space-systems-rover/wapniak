#pragma once

#include "data.h"
#include "queue.h"
#include "transformation.h"
#include <GLFW/glfw3.h>
#include <locale>
#include <mutex>

struct GuiApp {
  GuiApp(Queue<RawData>& queue);
  ~GuiApp();

  bool should_close() const;
  void draw();

  Queue<RawData> *get_ingest_queue();

private:
  GLFWwindow *window;
  Queue<RawData>& ingest_queue;
  // DrawableData to_draw{};
  CircularBuffer<NormalizedData> last_n_samples{100};
  std::array<Transformation, 2> transformations = {
       Transformation{"Live data", live_data, true},
       Transformation{"Average", avg, false},
  };

  void consume_data_queue();
};
