// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <chrono>
#include <format>
#include <vector>
#include <deque>
#include <filesystem>
#include <fstream>

#define FMT_HEADER_ONLY
#include <fmt/format.h>
#include <fmt/color.h>
#include <fmt/xchar.h>
#include <fmt/chrono.h>

#include "spdlog/spdlog.h"


#include "gl/glew.h"
#include "GLFW/glfw3.h"
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3native.h"

#include "glm/glm.hpp"

//#include "opencv2/opencv.hpp"

using namespace std::literals;
