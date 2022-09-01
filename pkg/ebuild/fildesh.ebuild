EAPI=8

CMAKE_BUILD_TYPE="Release"
CMAKE_ECLASS="cmake"
inherit cmake

DESCRIPTION="Now you're scripting with FIFOs."
HOMEPAGE="https://github.com/fildesh/fildesh"
SRC_URI="https://github.com/fildesh/fildesh/archive/refs/tags/v${PV}.tar.gz -> ${P}.tar.gz"

LICENSE="ISC"
SLOT="0/0"
KEYWORDS="~amd64 ~x86"
IUSE=""

