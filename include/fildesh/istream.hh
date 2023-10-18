#ifndef FILDESH_ISTREAM_HH_
#define FILDESH_ISTREAM_HH_
#include <istream>

#include <fildesh/streambuf.hh>

namespace fildesh {

class istream : public std::istream
{
public:
  explicit istream(::FildeshX* in)
    : std::istream(&inbuf_)
    , inbuf_(in)
  {
    if (!inbuf_) {setstate(std::ios::badbit);}
  }

  virtual ~istream() {}

  FildeshX* c_struct() {return inbuf_.c_struct();}
  const FildeshX* c_struct() const {return inbuf_.c_struct();}

protected:
  void reset(FildeshX* in) {
    clear();
    inbuf_.reset(in);
    if (!inbuf_) {setstate(std::ios::badbit);}
  }

private:
  fildesh::istreambuf inbuf_;
};


class ifstream : public istream
{
public:
  ifstream()
    : istream(NULL)
  {}

  explicit ifstream(const char* filename)
    : istream(::open_FildeshXF(filename))
  {}

  void open(const char* filename) {
    reset(::open_FildeshXF(filename));
  }

  void close() {
    reset(NULL);
  }
};

}  // namespace fildesh
#endif
