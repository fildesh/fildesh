EAPI=8

CMAKE_BUILD_TYPE="Release"
CMAKE_ECLASS="cmake"
inherit cmake

DESCRIPTION="Now you're scripting with FIFOs."
HOMEPAGE="https://github.com/fildesh/fildesh"
SRC_URI="https://github.com/fildesh/fildesh/releases/download/v${PV}/fildesh-${PV}.tar.gz -> ${P}.tar.gz"

LICENSE="ISC"
SLOT="0/0"
KEYWORDS="~amd64 ~x86"
IUSE="vim-syntax"

src_install() {
  cmake_src_install
  rm "${ED}"/usr/lib*/fildesh/libfildeshcompat*
  rm "${ED}"/usr/include/fildesh/fildesh_compat*
  if ! use vim-syntax; then
    rm -r "${ED}/usr/share"
  fi
}
