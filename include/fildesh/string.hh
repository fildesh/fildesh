#ifndef FILDESH_STRING_HH_
#define FILDESH_STRING_HH_
#include <string>

#include <fildesh/ostream.hh>


namespace fildesh {
inline std::string make_string(FildeshX slice) {
  return std::string(&slice.at[slice.off], slice.size - slice.off);
}
inline std::string make_string(FildeshO slice) {
  return std::string(&slice.at[slice.off], slice.size - slice.off);
}

#ifdef __cpp_lib_string_view
constexpr std::string_view make_string_view(FildeshX slice) {
  return std::string_view(&slice.at[slice.off], slice.size - slice.off);
}
constexpr std::string_view make_string_view(FildeshO slice) {
  return std::string_view(&slice.at[slice.off], slice.size - slice.off);
}
#endif  // defined(__cpp_lib_string_view)
}  // namespace fildesh


inline FildeshO& operator<<(FildeshO& out, FildeshX slice) {
  putslice_FildeshO(&out, slice);
  return out;
}
#ifdef __cpp_lib_string_view
inline FildeshO& operator<<(FildeshO& out, std::string_view s) {
  put_bytestring_FildeshO(&out, (unsigned char*)s.data(), s.size());
  return out;
}
#endif  // defined(__cpp_lib_string_view)


namespace fildesh {

inline bool slurp_file_to_string(std::string& text, const char* filename) {
  FildeshX* in = open_FildeshXF(filename);
  slurp_FildeshX(in);
  if (in && in->at) {
    text.assign(in->at, in->size);
  }
  else {
    text.clear();
  }
  close_FildeshX(in);
  return !!in;
}

class ostringstream : public ostream
{
public:
  ostringstream()
    : ostream(&oslice_)
    , oslice_(default_FildeshO())
  {}

  char* c_str() {
    putc_FildeshO(&oslice_, '\0');
    oslice_.size -= 1;
    return &oslice_.at[oslice_.off];
  }
  std::string str() const {
    return make_string(oslice_);
  }
#ifdef __cpp_lib_string_view
  std::string_view view() const {
    return make_string_view(oslice_);
  }
#endif  // defined(__cpp_lib_string_view)

  void truncate() {truncate_FildeshO(&oslice_);}

private:
  FildeshO oslice_;
};

}  // namespace fildesh
#endif
