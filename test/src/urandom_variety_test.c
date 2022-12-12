/** Test that runs until it generates 256 uniquue byte values randomly.**/
#include <fildesh/fildesh.h>
#include <assert.h>
#include <string.h>

int main() {
  unsigned char seen[256];
  unsigned unique_count = 0;
  FildeshX* in = open_FildeshXF("/dev/urandom");
  assert(in);

  memset(seen, 0, sizeof(seen));

  while (unique_count < 256) {
    unsigned char b;
    if (in->off == in->size) {
      size_t nread = read_FildeshX(in);
      assert(nread > 0);
    }
    b = (unsigned char)in->at[in->off++];
    if (!seen[b]) {
      seen[b] = 1;
      unique_count += 1;
    }
  }

  close_FildeshX(in);
  return 0;
}
