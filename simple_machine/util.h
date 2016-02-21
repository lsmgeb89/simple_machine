#ifndef SIMPLE_MACHINE_UTIL_H_
#define SIMPLE_MACHINE_UTIL_H_

#include <iomanip>

#ifndef NDEBUG
#define debug_clog std::clog

#define MODULE_SYSTEM "[system]"
#define MODULE_CPU "[cpu]"
#define MODULE_MEMORY "[memory]"
#define MODULE_MESSAGE "[message]"

#define LOG_ERROR "[error]"
#define LOG_INFO "[info]"
#else
class NullBuffer : public std::streambuf {
public:
  int overflow(int c) { return c; }
};

class NullStream : public std::ostream {
  public:
    NullStream() : std::ostream(&m_sb) {}
  private:
    NullBuffer m_sb;
};
static NullStream null_stream;

#define debug_clog null_stream

#define MODULE_SYSTEM ""
#define MODULE_CPU ""
#define MODULE_MEMORY ""
#define MODULE_MESSAGE ""

#define LOG_ERROR ""
#define LOG_INFO ""
#endif

#define info_system debug_clog << LOG_INFO << MODULE_SYSTEM
#define info_cpu debug_clog << LOG_INFO << MODULE_CPU
#define info_memory debug_clog << LOG_INFO << MODULE_MEMORY
#define info_message debug_clog << LOG_INFO << MODULE_MESSAGE

#define error_system std::cerr << LOG_ERROR << MODULE_SYSTEM
#define error_cpu std::cerr << LOG_ERROR << MODULE_CPU
#define error_memory std::cerr << LOG_ERROR << MODULE_MEMORY
#define error_message std::cerr << LOG_ERROR << MODULE_MESSAGE

#define LOG_ERROR_MODULE_SYSTEM LOG_ERROR MODULE_SYSTEM

#endif // SIMPLE_MACHINE_MCHN_UTIL_H_
