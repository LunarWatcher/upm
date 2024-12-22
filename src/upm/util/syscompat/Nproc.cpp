#include "Nproc.hpp"

#include "upm/platform/Platform.hpp"
#include <spdlog/spdlog.h>

#ifdef LINUX
#include <unistd.h>
#endif

extern int upm::util::getHardwareConcurrency() {
#ifdef LINUX
    long procs = sysconf(_SC_NPROCESSORS_ONLN);
    if (procs < 1) {
        spdlog::warn("Failed to get hardware concurrency. Defaulting to 1");
        return 1;
    }
    return (int) procs;
#else
#warning "Nproc compat is not implemented for your system. Please consider contributing a patch. Defaulting to 1 instead"
    return 1;
#endif
}
