#include "gui_app.h"
#include "data.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
#include "static_data.h"
#include "transformation.h"
#include <GLFW/glfw3.h>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <stdexcept>

static void glfw_error_callback(int error, const char *description) {
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

GuiApp::GuiApp(Queue<RawData> &queue) : ingest_queue(queue) {
  // Setup GLFW
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) {
    throw std::runtime_error("Failed to initialize glfw");
  }

  // GL 3.0 + GLSL 130
  constexpr char const *glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);

  // Create window with graphics context
  window = glfwCreateWindow(1280, 720, "Spectrograph", NULL, NULL);
  if (window == NULL) {
    throw std::runtime_error("Failed to create window");
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1); // Enable vsync

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImPlot::CreateContext(); // Create ImPlot context

  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  ImGui::StyleColorsDark(); // Dark theme

  ImFontConfig font_cfg;
  font_cfg.FontDataOwnedByAtlas = false;
  io.Fonts->AddFontFromMemoryTTF(my_font, my_font_length, 20, &font_cfg);

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);
}

GuiApp::~GuiApp() {
  // Cleanup
  ImPlot::DestroyContext();
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();
}

void GuiApp::draw() {
  glfwPollEvents();

  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

#if 1
  float xscale{2}, yscale{2};
  glfwGetWindowContentScale(window, &xscale, &yscale);

  ImGuiIO &io = ImGui::GetIO();
  // io.DisplayFramebufferScale = ImVec2(xscale, yscale);

  int width, height;
  glfwGetWindowSize(window, &width, &height);
  // io.DisplaySize = ImVec2((float)width * xscale, (float)height * yscale);
  io.DisplaySize = ImVec2((float)width, (float)height);

  printf("Scale: %f, %f\n", xscale, yscale);

  // If you haven't already adjusted your fonts, consider:
  io.FontGlobalScale = xscale;
#endif

  // Get the size of the window and create a window that spans the entire area
  int display_w, display_h;
  glfwGetFramebufferSize(window, &display_w, &display_h);
  display_w /= xscale;
  display_h /= yscale;

  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize(ImVec2((float)display_w, (float)display_h));
  ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
      ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
      ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
      ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

  ImGui::Begin("FullWindow", NULL, window_flags);

  consume_data_queue();
  const std::vector<NormalizedData> last_samples = last_n_samples.getAll();

  float totalHeight = ImGui::GetContentRegionAvail().y;
  float rowHeight = totalHeight / 2;
  if (ImGui::BeginChild("Row1", ImVec2(0, rowHeight), true)) {
    ImPlotFlags flags = ImPlotFlags_NoInputs;
    if (ImPlot::BeginPlot("Graph", ImVec2(-1, -1), flags)) {
      ImPlot::SetupAxes("Wavelength (nm)", "Light Intensity");
      ImPlot::SetupAxesLimits(RawData::min_wavelength, RawData::max_wavelength,
                              0, 1.0);

      for (const Transformation &transform : transformations) {
        if (!transform.enabled) {
          continue;
        }

        const DrawableData data = transform.func(last_samples);

        ImPlot::PlotLine(transform.name.c_str(), data.xs.data(), data.ys.data(),
                         data.xs.size());
      }

      ImPlot::EndPlot();
    }
  }
  ImGui::EndChild();

  // Second row
  if (ImGui::BeginChild("Row2", ImVec2(0, rowHeight), true)) {
    ImGui::Text("Select which transformations to apply:");
    for (auto &t : transformations) {
      ImGui::Checkbox(t.name.c_str(), &t.enabled);
    }
  }
  ImGui::EndChild();

  ImGui::End();

  // Rendering
  ImGui::Render();
  glViewport(0, 0, display_w, display_h);
  glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
  glClear(GL_COLOR_BUFFER_BIT);
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  glfwSwapBuffers(window);
}

bool GuiApp::should_close() const { return glfwWindowShouldClose(window); }

Queue<RawData> *GuiApp::get_ingest_queue() { return &ingest_queue; }

void GuiApp::consume_data_queue() {
  const std::vector<RawData> samples = ingest_queue.get_all();
  if (samples.empty()) {
    return;
  }
  for (const RawData &sample : samples) {
    last_n_samples.push(NormalizedData{sample});
  }
}
