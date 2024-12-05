#pragma once

// Platform specific definitions
#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#else
#include <signal.h>
#include <unistd.h>
#endif

// Standard C++ headers
#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <functional>
#include <future>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <random>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

// Logging
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

// Third-party libraries
#include "INIReader/INIReader.h"

// GLFW
#include "GLFW/glfw3.h"