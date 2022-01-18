
#include "fildesh.h"
#include "fildesh_builtin.h"
#include "cx/syscx.h"

int main(int argc, char** argv)
{
  int exstatus;
  init_sysCx();
  exstatus = fildesh_builtin_fildesh_main((unsigned)argc, argv, NULL, NULL);
  lose_sysCx();
  return exstatus;
}

