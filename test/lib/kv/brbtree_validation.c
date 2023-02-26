#include "brbtree_validation.h"

#include "rbtree_validation.h"
#include "src/lib/kv_bstree.h"


/** Verify that the map is fairly compact.
 *
 * As long as keys are small enough, our broaadleaf red-black tree
 * makes sure to keep a node/entry ratio less than 75% in every subtree.
 *
 * In terms of compression factor, this means that a broadleaf red-black tree
 * is 25% smaller than a conventional red-black tree.
 * In terms of capability, the broadleaf red-black tree can hold 1/3 more nodes
 * (that's not 1/3 more data unless keys & values are stored directly in nodes).
 **/
  void
validate_FildeshKV_BRBTREE(const FildeshKV* map)
{
  bool all_keys_small_enough = true;
  size_t id_count = 0;
  size_t node_count = 0;
  FildeshKV_id_t id;

  for (id = first_FildeshKV(map);
       !fildesh_nullid(id);
       id = next_at_FildeshKV(map, id))
  {
    size_t x = id/2;
    id_count += 1;
    if ((id & 1) == 0) {
      node_count += 1;
    }
    if (ksize_FildeshKVE_size(map->at[x].size) > FildeshKVE_splitksize_max) {
      all_keys_small_enough = false;
    }
  }
  if (id_count > 1 && all_keys_small_enough) {
    /* Guarantee 25% compression.*/
    assert(4*node_count <= 3*id_count);
    if (id_count != 4) {
      /* Strict inequality, usually.*/
      assert(4*node_count < 3*id_count);
    }
  }

  if (id_count != 4) {
    /* Size-4 case is off-balance.*/
    validate_FildeshKV_RBTREE(map);
  }
}
