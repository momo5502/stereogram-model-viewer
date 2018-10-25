#pragma once

#define NOMINMAX
#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#define _USE_MATH_DEFINES

#ifdef _WIN32
#include <windows.h>
#endif

#include <math.h>
#include <stdio.h>
#include <string.h>

#pragma warning(push)
#pragma warning(disable: 4244)
#include <map>
#include <list>
#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <fstream>
#include <random>

#include <gsl/gsl>

#define GLM_FORCE_SILENT_WARNINGS 1
#define GLM_ENABLE_EXPERIMENTAL 1
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#pragma warning(pop)

using namespace std::literals;
