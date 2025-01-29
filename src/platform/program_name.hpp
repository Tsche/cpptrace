#ifndef PROGRAM_NAME_HPP
#define PROGRAM_NAME_HPP

#include <mutex>
#include <string>

#include "platform/platform.hpp"

#if IS_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
 #define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#define CPPTRACE_MAX_PATH MAX_PATH

namespace cpptrace {
namespace detail {
    inline const char* program_name() {
        static std::mutex mutex;
        const std::lock_guard<std::mutex> lock(mutex);
        static std::string name;
        static bool did_init = false;
        static bool valid = false;
        if(!did_init) {
            did_init = true;
            char buffer[MAX_PATH + 1];
            int res = GetModuleFileNameA(nullptr, buffer, MAX_PATH);
            if(res) {
                name = buffer;
                valid = true;
            }
        }
        return valid && !name.empty() ? name.c_str() : nullptr;
    }
}
}

#elif IS_APPLE

#include <cstdint>
// #include <mach-o/dyld.h>
#include <sys/syslimits.h>

// https://github.com/opensource-apple/dyld/blob/3f928f32597888c5eac6003b9199d972d49857b5/include/mach-o/dyld.h#L92C1-L92C62
// https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man3/dyld.3.html
extern int _NSGetExecutablePath(char* buf, uint32_t* bufsize);

#define CPPTRACE_MAX_PATH CPPTRACE_PATH_MAX

namespace cpptrace {
namespace detail {
    inline const char* program_name() {
        static std::mutex mutex;
        const std::lock_guard<std::mutex> lock(mutex);
        static std::string name;
        static bool did_init = false;
        static bool valid = false;
        if(!did_init) {
            did_init = true;
            char buffer[CPPTRACE_PATH_MAX + 1];
            std::uint32_t bufferSize = sizeof buffer;
            if(_NSGetExecutablePath(buffer, &bufferSize) == 0) {
                name.assign(buffer, bufferSize);
                valid = true;
            }
        }
        return valid && !name.empty() ? name.c_str() : nullptr;
    }
}
}

#elif IS_LINUX

#include <linux/limits.h>
#include <sys/types.h>
#include <unistd.h>

#define CPPTRACE_MAX_PATH CPPTRACE_PATH_MAX

namespace cpptrace {
namespace detail {
    inline const char* program_name() {
        static std::mutex mutex;
        const std::lock_guard<std::mutex> lock(mutex);
        static std::string name;
        static bool did_init = false;
        static bool valid = false;
        if(!did_init) {
            did_init = true;
            char buffer[CPPTRACE_PATH_MAX + 1];
            const ssize_t size = readlink("/proc/self/exe", buffer, CPPTRACE_PATH_MAX);
            if(size == -1) {
                return nullptr;
            }
            buffer[size] = 0;
            name = buffer;
            valid = true;
        }
        return valid && !name.empty() ? name.c_str() : nullptr;
    }
}
}

#endif

#endif
