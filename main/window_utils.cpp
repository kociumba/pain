#include "window_utils.h"

using std::max;
using std::min;

GLFWmonitor *get_current_monitor(GLFWwindow *window) {
    int nmonitors;
    auto monitors = glfwGetMonitors(&nmonitors);

    if (!monitors || nmonitors == 0) {
        return NULL;
    }

    int window_x, window_y, window_width, window_height;
    glfwGetWindowPos(window, &window_x, &window_y);
    glfwGetWindowSize(window, &window_width, &window_height);

    int best_overlap_area = 0;
    GLFWmonitor *best_monitor = NULL;

    for (int i = 0; i < nmonitors; ++i) {
        GLFWmonitor *monitor_candidate = monitors[i];

        int monitor_x, monitor_y;
        glfwGetMonitorPos(monitor_candidate, &monitor_x, &monitor_y);

        const GLFWvidmode *mode = glfwGetVideoMode(monitor_candidate);
        int monitor_width = mode->width;
        int monitor_height = mode->height;

        // Calculate overlap
        int overlap_left = max(window_x, monitor_x);
        int overlap_top = max(window_y, monitor_y);
        int overlap_right = min(window_x + window_width, monitor_x + monitor_width);
        int overlap_bottom = min(window_y + window_height, monitor_y + monitor_height);

        int overlap_width = max(0, overlap_right - overlap_left);
        int overlap_height = max(0, overlap_bottom - overlap_top);

        int current_overlap_area = overlap_width * overlap_height;

        if (current_overlap_area > best_overlap_area) {
            best_overlap_area = current_overlap_area;
            best_monitor = monitor_candidate;
        }
    }

    return best_monitor;
}