#include "data.h"
#include "gui_app.h"
#include <GLFW/glfw3.h>
#include <chrono>
#include <thread>

void shit_poster(Queue<RawData> &queue, bool &run) {
  RandomDataGenerator generator{};
  while (run) {
    RawData data = generator.generate();
    queue.push(data);
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

int main(int, char **) {
  Queue<RawData> ingest_queue{100};
  bool worker_should_run{true};
  std::thread worker{shit_poster, std::ref(ingest_queue),
                     std::ref(worker_should_run)};
  GuiApp app{ingest_queue};
  while (!app.should_close()) {
    app.draw();
  }
  worker_should_run = false;
  worker.join();
  return 0;
}
