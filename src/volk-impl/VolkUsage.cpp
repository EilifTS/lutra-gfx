#if defined(_WIN32)
#   define VK_USE_PLATFORM_WIN32_KHR
#elif defined(__linux__) || defined(__unix__)
#   define VK_USE_PLATFORM_XLIB_KHR
#elif defined(__APPLE__)
#   define VK_USE_PLATFORM_METAL_EXT
#else
#   error "Unsupported platform."
#endif

#define VOLK_IMPLEMENTATION
#include <volk.h>