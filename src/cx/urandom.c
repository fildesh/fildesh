/** TODO: Move to src/ and wrap in a FildeshX.**/

#include "lace_compat_random.h"
#include "urandom.h"

/** If our uint32 type is more than 32 bits, then this mask
 * should be applied to ensure overflowing sums are truncated.
 * But uint32_t should be 32 bits, so it isn't used.
 **/
#define MASK32(x)  (UINT32_MAX & (x))
#define MASK16(x)  (UINT16_MAX & (x))


typedef struct URandom URandom;
struct URandom {
  uint32 state[17];
  uint32 salt;
  /* uint sys_pcidx; */
  /* uint sys_npcs; */
};

/** Chris Lomont's random number generator.
 * From: http://stackoverflow.com/a/1227137/5039395
 * http://lomont.org/papers/2008/Lomont_PRNG_2008.pdf
 **/
qual_inline
  uint32_t
uint32_WELL512 (URandom* ctx)
{
  uint32_t* state = ctx->state;
#define index  ctx->state[16]
  uint32_t a, b, c, d;
  a = state[index];
  c = state[(index+13)&15];
  b = a^c^(a<<16)^(c<<15);
  c = state[(index+9)&15];
  c ^= (c>>11);
  a = state[index] = b^c;
  d = a^((a<<5)&0xDA442D24UL);
  index = (index + 15)&15;
  a = state[index];
  state[index] = a^b^d^(a<<2)^(b<<18)^(c<<28);
  return state[index];
}

qual_inline
  void
init_WELL512 (URandom* ctx)
{
  index = 0;
#undef index
}


/** 32-bit hash function from Thomas Wang.
 *
 * Found here: http://burtleburtle.net/bob/hash/integer.html
 **/
qual_inline
  uint32_t
uint32_hash_ThomasWang (uint32_t a)
{
  a = (a ^ 61) ^ (a >> 16);
  a = a + (a << 3);
  a = a ^ (a >> 4);
  a = a * 0x27d4eb2d;
  a = a ^ (a >> 15);
  return a;
}
#define uint32_hash uint32_hash_ThomasWang


/** 64-bit hash function from Thomas Wang.
 *
 * Found here: https://naml.us/blog/tag/thomas-wang
 **/
qual_inline
  uint64_t
uint64_hash_ThomasWang(uint64_t key)
{
  key = (~key) + (key << 21); /* key = (key << 21) - key - 1;*/
  key = key ^ (key >> 24);
  key = (key + (key << 3)) + (key << 8); /* key * 265 */
  key = key ^ (key >> 14);
  key = (key + (key << 2)) + (key << 4); /* key * 21 */
  key = key ^ (key >> 28);
  key = key + (key << 31);
  return key;
}


  void
init2_seeded_URandom (URandom* urandom, uint pcidx, uint npcs)
{
  (void) npcs;
  init_WELL512 (urandom);
  /* init_GMRand (urandom); */
  urandom->salt = uint32_hash(pcidx);
}

  void
init3_URandom (URandom* urandom, uint pcidx, uint npcs, uint seed)
{
  uint i;
  (void) npcs;
  for (i = 0; i < ArraySz(urandom->state); ++i) {
    uint32 x = seed + i + ArraySz(urandom->state) * pcidx;
    urandom->state[i] = uint32_hash(x);
  }

  init2_seeded_URandom (urandom, pcidx, npcs);
}

  void
init2_URandom (URandom* urandom, uint pcidx, uint npcs)
{
  uint i;
  (void) npcs;
  for (i = 0; i < ArraySz(urandom->state); ++i) {
    uint32 x = i + ArraySz(urandom->state) * pcidx;
    urandom->state[i] = uint32_hash(x);
  }

  i = lace_compat_random_bytes(urandom->state, sizeof(urandom->state));
  assert(i == sizeof(urandom->state));
  init2_seeded_URandom (urandom, pcidx, npcs);
}

  void
init1_URandom (URandom* urandom, uint seed)
{
  init3_URandom (urandom, 0, 1, seed);
}

  uint32
uint32_URandom (URandom* urandom)
{
  uint32 x = uint32_WELL512 (urandom);
  /* uint32 x = uint32_GMRand (urandom); */
  return (x ^ urandom->salt);
}
