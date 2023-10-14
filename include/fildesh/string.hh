#ifndef FILDESH_STRING_HH_
#define FILDESH_STRING_HH_
#include <fildesh/fildesh.h>

#include <string>


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
#endif
