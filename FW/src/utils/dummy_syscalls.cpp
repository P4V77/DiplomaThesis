#include "logging/Logger.hpp"
#include <newlib.h>
#include <sys/stat.h>

namespace l
{
     namespace
     {
          auto log = []() -> nd::utils::Logger &
          {
               return nd::utils::Logger::getLogger();
          };
     }
}

//
// Dummy syscalls to satisfy linker
//

extern "C" int _close()
{
     l::log().error("Dummy syscall _close!");
     return -1;
}

extern "C" int _getpid(int)
{
     l::log().error("Dummy syscall _getpid!");
     return -1;
}

extern "C" int _kill(int, int)
{
     l::log().error("Dummy syscall _kill!");
     return -1;
}

extern "C" int _lseek(int, int, int)
{
     l::log().error("Dummy syscall _lseek!");
     return -1;
}

extern "C" int _read(int, char *, int)
{
     l::log().error("Dummy syscall _read!");
     return -1;
}

extern "C" int _write(int, char *, int)
{
     l::log().error("Dummy syscall _write!");
     return -1;
}

extern "C" void *__wrap_malloc(size_t)
{
     l::log().error("Dummy syscall _malloc!");
     return nullptr;
}

extern "C" int _fstat(int fd, struct stat *st)
{
     l::log().error("Dummy syscall _fstat!");
     (void)fd;
     if (st != nullptr)
     {
          st->st_mode = S_IFCHR; // Set as character device
     }
     return 0;
}

extern "C" int _isatty(int fd)
{
     (void)fd; // Mark parameter as intentionally unused
     l::log().error("Dummy syscall _isatty!");
     return 1; // Always return true
}