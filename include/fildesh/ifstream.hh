#ifndef FILDESH_IFSTREAM_HH_
#define FILDESH_IFSTREAM_HH_
#include <fildesh/fildesh.h>

#include <cstring>
#include <istream>

#ifndef fildesh_cc_override
# if defined(__cplusplus) && (__cplusplus >= 201103L)
#  define fildesh_cc_override override
# else
#  define fildesh_cc_override
# endif
#endif


namespace fildesh {
class ifstream : private std::streambuf, public std::istream
{
public:
  ifstream()
  : std::istream(this)
  , in_(NULL)
  {
    setstate(std::ios::badbit);
  }

  ifstream(const std::string& filename)
    : std::istream(this)
    , in_(::open_FildeshXF(filename.c_str()))
  {
    if (!in_) {setstate(std::ios::badbit);}
  }

  ifstream(::FildeshX* in)
    : std::istream(this)
    , in_(in)
  {
    if (!in_) {setstate(std::ios::badbit);}
  }

  ~ifstream() {
    this->close();
  }

  void close() {
    setstate(std::ios::badbit);
    close_FildeshX(in_);
    in_ = NULL;
  }
  void open(const std::string& filename) {
    clear();
    close_FildeshX(in_);
    in_ = ::open_FildeshXF(filename.c_str());
  }

private:
  std::streamsize xsgetn(char* buf, std::streamsize size) fildesh_cc_override {
    if (in_->off >= in_->size) {
      maybe_flush_FildeshX(in_);
      read_FildeshX(in_);
    }
    if ((std::streamsize)(in_->size - in_->off) < size) {
      size = in_->size - in_->off;
    }
    memcpy(buf, &in_->at[in_->off], size);
    in_->off += size;
    return size;
  }

  std::streambuf::int_type uflow() fildesh_cc_override {
    char buf[1];
    std::streamsize n = xsgetn(buf, 1);
    if (n == 0) {
      return std::streambuf::traits_type::eof();
    }
    return buf[0];
  }

  std::streambuf::int_type underflow() fildesh_cc_override {
    if (in_->off >= in_->size) {
      maybe_flush_FildeshX(in_);
      read_FildeshX(in_);
    }
    if (in_->off < in_->size) {
      return in_->at[in_->off];
    }
    return std::streambuf::traits_type::eof();
  }

  std::streambuf::int_type pbackfail(std::streambuf::int_type c) fildesh_cc_override {
    if (in_->off == 0) {
      return std::streambuf::traits_type::eof();
    }
    in_->off -= 1;
    if (c != std::streambuf::traits_type::eof()) {
      in_->at[in_->off] = c;
    }
    else {
      c = in_->at[in_->off];
    }
    return c;
  }

  std::streamsize showmanyc() fildesh_cc_override {
    return in_->size - in_->off;
  }

private:
  FildeshX* in_;
};
}
#endif
