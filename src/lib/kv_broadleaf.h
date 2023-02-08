#include <fildesh/fildesh.h>

BEGIN_EXTERN_C

FildeshKV_id_t
ensure_FildeshKV_BROADLEAF_BSTREE(FildeshKV*, const void*, size_t, FildeshAlloc*);

FildeshKV_id_t
maybe_redden_fuse_FildeshKV_BROADLEAF_RBTREE(
    FildeshKV* map, size_t b, FildeshKV_id_t insertion_id);
void
maybe_fuse_FildeshKV_BROADLEAF_RBTREE(FildeshKV* map, size_t b);

bool
fixup_remove_case_0_FildeshKV_BROADLEAF_RBTREE(
    FildeshKV* map, size_t b, size_t a);

void
fixup_remove_case_1_FildeshKV_BROADLEAF_RBTREE(
    FildeshKV*, size_t y, unsigned side,
    size_t b, size_t a, size_t x);

END_EXTERN_C
