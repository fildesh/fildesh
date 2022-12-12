#ifndef FILDESH_OFSTREAM_HH_
#define FILDESH_OFSTREAM_HH_
#include <fildesh/fildesh.h>

#include <cstring>
#include <ostream>

#ifndef fildesh_cc_override
# if defined(__cplusplus) && (__cplusplus >= 201103L)
#  define fildesh_cc_override override
# else
#  define fildesh_cc_override
# endif
#endif


namespace fildesh {
class ofstream : private std::streambuf, public std::ostream
{
public:
  ofstream()
    : std::ostream(this)
    , out_(NULL)
  {
    setstate(std::ios::badbit);
  }

  ofstream(const std::string& filename)
    : std::ostream(this)
    , out_(::open_FildeshOF(filename.c_str()))
  {
    if (!out_) {setstate(std::ios::badbit);}
  }

  ofstream(::FildeshO* out)
    : std::ostream(this)
    , out_(out)
  {
    if (!out_) {setstate(std::ios::badbit);}
  }

  ~ofstream() {
    this->close();
  }

  void close() {
    setstate(std::ios::badbit);
    close_FildeshO(out_);
    out_ = NULL;
  }
  void open(const std::string& filename) {
    clear();
    close_FildeshO(out_);
    out_ = ::open_FildeshOF(filename.c_str());
  }

private:
  int sync() fildesh_cc_override {
    ::flush_FildeshO(out_);
    return 0;
  }

  std::streamsize xsputn(const char* buf, std::streamsize size) fildesh_cc_override {
    memcpy(grow_FildeshO(out_, size), buf, size);
    maybe_flush_FildeshO(out_);
    return size;
  }

  std::streambuf::int_type overflow(std::streambuf::int_type c) fildesh_cc_override {
    ::putc_FildeshO(out_, (char)c);
    return 0;
  }

private:
  FildeshO* out_;
};
}
#endif
