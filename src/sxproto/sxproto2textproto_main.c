#include <fildesh/sxproto.h>

int main()
{
  FildeshX* in = open_fd_FildeshX(0);
  FildeshO* out = open_fd_FildeshO(1);
  int exstatus = 0;

  if (!sxproto2textproto(in, out)) {
    exstatus = 1;
  }
  close_FildeshO(out);
  return exstatus;
}
