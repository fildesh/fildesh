#ifndef FILDESH_STREAMBUF_HH_
#define FILDESH_STREAMBUF_HH_
#include <cstring>
#include <streambuf>

#include <fildesh/fildesh.h>

#ifndef fildesh_cc_override
# if defined(__cplusplus) && (__cplusplus >= 201103L)
#  define fildesh_cc_override override
# else
#  define fildesh_cc_override
# endif
#endif

namespace fildesh {

class istreambuf : public std::streambuf
{
public:
  istreambuf()
    : in_(NULL)
  {}

  explicit istreambuf(::FildeshX* in)
    : in_(in)
  {}

  ~istreambuf() {
    reset(NULL);
  }

  void reset(FildeshX* in) {
    close_FildeshX(in_);
    in_ = in;
  }

  bool operator!() const {return !in_;}
  FildeshX* c_struct() {return in_;}
  const FildeshX* c_struct() const {return in_;}

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


class ostreambuf : public std::streambuf
{
public:
  ostreambuf()
    : out_(NULL)
  {}

  explicit ostreambuf(::FildeshO* out)
    : out_(out)
  {}

  ~ostreambuf() {
    reset(NULL);
  }

  void reset(FildeshO* out) {
    close_FildeshO(out_);
    out_ = out;
  }

  bool operator!() const {return !out_;}
  FildeshO* c_struct() {return out_;}
  const FildeshO* c_struct() const {return out_;}

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

}  // namespace fildesh
#endif
