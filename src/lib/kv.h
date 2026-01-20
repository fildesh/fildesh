#include "kve.h"
#include <assert.h>

static const FildeshKV_id_t FildeshKV_NULL_ID = ~(size_t)0;

#define assert_trivial_joint(joint)  assert(joint == get_index_FildeshKVE_joint(joint))

BEGIN_EXTERN_C

extern const FildeshKV_VTable DEFAULT_BSTREE_FildeshKV_VTable;
#define DEFAULT_FildeshKV_BSTREE { NULL, NULL, 0, 0, &DEFAULT_BSTREE_FildeshKV_VTable }
extern const FildeshKV_VTable DEFAULT_RBTREE_FildeshKV_VTable;
#define DEFAULT_FildeshKV_RBTREE { NULL, NULL, 0, 0, &DEFAULT_RBTREE_FildeshKV_VTable }
extern const FildeshKV_VTable DEFAULT_BROADLEAF_BSTREE_FildeshKV_VTable;
#define DEFAULT_FildeshKV_BROADLEAF_BSTREE { NULL, NULL, 0, 0, &DEFAULT_BROADLEAF_BSTREE_FildeshKV_VTable }
extern const FildeshKV_VTable DEFAULT_BROADLEAF_RBTREE_FildeshKV_VTable;
#define DEFAULT_FildeshKV_BROADLEAF_RBTREE { NULL, NULL, 0, 0, &DEFAULT_BROADLEAF_RBTREE_FildeshKV_VTable }

void maybe_grow_FildeshKV_SINGLE_LIST(FildeshKV*);
void reclaim_element_FildeshKV_SINGLE_LIST(FildeshKV*, size_t);

END_EXTERN_C
