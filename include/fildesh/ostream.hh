#ifndef FILDESH_OSTREAM_HH_
#define FILDESH_OSTREAM_HH_
#include <ostream>

#include <fildesh/streambuf.hh>

namespace fildesh {

class ostream : public std::ostream
{
public:
  explicit ostream(::FildeshO* out)
    : std::ostream(&outbuf_)
    , outbuf_(out)
  {
    if (!outbuf_) {setstate(std::ios::badbit);}
  }

  virtual ~ostream() {}

  FildeshO* c_struct() {return outbuf_.c_struct();}
  const FildeshO* c_struct() const {return outbuf_.c_struct();}

protected:
  void reset(FildeshO* out) {
    clear();
    outbuf_.reset(out);
    if (!outbuf_) {setstate(std::ios::badbit);}
  }

private:
  fildesh::ostreambuf outbuf_;
};


class ofstream : public ostream
{
public:
  ofstream()
    : ostream(NULL)
  {}

  explicit ofstream(const char* filename)
    : ostream(::open_FildeshOF(filename))
  {}

  void open(const char* filename) {
    reset(::open_FildeshOF(filename));
  }

  void close() {
    reset(NULL);
  }
};

}  // namespace fildesh
#endif
