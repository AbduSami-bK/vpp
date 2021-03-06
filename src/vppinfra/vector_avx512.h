/*
 * Copyright (c) 2015 Cisco and/or its affiliates.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef included_vector_avx512_h
#define included_vector_avx512_h

#include <vppinfra/clib.h>
#include <x86intrin.h>

/* *INDENT-OFF* */
#define foreach_avx512_vec512i \
  _(i,8,64,epi8) _(i,16,32,epi16) _(i,32,16,epi32)  _(i,64,8,epi64)
#define foreach_avx512_vec512u \
  _(u,8,64,epi8) _(u,16,32,epi16) _(u,32,16,epi32)  _(u,64,8,epi64)
#define foreach_avx512_vec512f \
  _(f,32,8,ps) _(f,64,4,pd)

/* splat, load_unaligned, store_unaligned, is_all_zero, is_equal,
   is_all_equal, is_zero_mask */
#define _(t, s, c, i) \
static_always_inline t##s##x##c						\
t##s##x##c##_splat (t##s x)						\
{ return (t##s##x##c) _mm512_set1_##i (x); }				\
\
static_always_inline t##s##x##c						\
t##s##x##c##_load_unaligned (void *p)					\
{ return (t##s##x##c) _mm512_loadu_si512 (p); }				\
\
static_always_inline void						\
t##s##x##c##_store_unaligned (t##s##x##c v, void *p)			\
{ _mm512_storeu_si512 ((__m512i *) p, (__m512i) v); }			\
\
static_always_inline int						\
t##s##x##c##_is_all_zero (t##s##x##c v)					\
{ return (_mm512_test_epi64_mask ((__m512i) v, (__m512i) v) == 0); }	\
\
static_always_inline int						\
t##s##x##c##_is_equal (t##s##x##c a, t##s##x##c b)			\
{ return t##s##x##c##_is_all_zero (a ^ b); }				\
\
static_always_inline int						\
t##s##x##c##_is_all_equal (t##s##x##c v, t##s x)			\
{ return t##s##x##c##_is_equal (v, t##s##x##c##_splat (x)); }		\
\
static_always_inline u##c						\
t##s##x##c##_is_zero_mask (t##s##x##c v)				\
{ return _mm512_test_##i##_mask ((__m512i) v, (__m512i) v); }		\


foreach_avx512_vec512i foreach_avx512_vec512u
#undef _
/* *INDENT-ON* */

static_always_inline u32
u16x32_msb_mask (u16x32 v)
{
  return (u32) _mm512_movepi16_mask ((__m512i) v);
}


static_always_inline void
u32x16_transpose (u32x16 m[16])
{
  __m512i r[16], a, b, c, d, x, y;

  /* *INDENT-OFF* */
  __m512i pm1 = (__m512i) (u64x8) { 0, 1, 8, 9, 4, 5, 12, 13};
  __m512i pm2 = (__m512i) (u64x8) { 2, 3, 10, 11, 6, 7, 14, 15};
  __m512i pm3 = (__m512i) (u64x8) { 0, 1, 2, 3, 8, 9, 10, 11};
  __m512i pm4 = (__m512i) (u64x8) { 4, 5, 6, 7, 12, 13, 14, 15};
  /* *INDENT-ON* */

  r[0] = _mm512_unpacklo_epi32 ((__m512i) m[0], (__m512i) m[1]);
  r[1] = _mm512_unpacklo_epi32 ((__m512i) m[2], (__m512i) m[3]);
  r[2] = _mm512_unpacklo_epi32 ((__m512i) m[4], (__m512i) m[5]);
  r[3] = _mm512_unpacklo_epi32 ((__m512i) m[6], (__m512i) m[7]);
  r[4] = _mm512_unpacklo_epi32 ((__m512i) m[8], (__m512i) m[9]);
  r[5] = _mm512_unpacklo_epi32 ((__m512i) m[10], (__m512i) m[11]);
  r[6] = _mm512_unpacklo_epi32 ((__m512i) m[12], (__m512i) m[13]);
  r[7] = _mm512_unpacklo_epi32 ((__m512i) m[14], (__m512i) m[15]);

  r[8] = _mm512_unpackhi_epi32 ((__m512i) m[0], (__m512i) m[1]);
  r[9] = _mm512_unpackhi_epi32 ((__m512i) m[2], (__m512i) m[3]);
  r[10] = _mm512_unpackhi_epi32 ((__m512i) m[4], (__m512i) m[5]);
  r[11] = _mm512_unpackhi_epi32 ((__m512i) m[6], (__m512i) m[7]);
  r[12] = _mm512_unpackhi_epi32 ((__m512i) m[8], (__m512i) m[9]);
  r[13] = _mm512_unpackhi_epi32 ((__m512i) m[10], (__m512i) m[11]);
  r[14] = _mm512_unpackhi_epi32 ((__m512i) m[12], (__m512i) m[13]);
  r[15] = _mm512_unpackhi_epi32 ((__m512i) m[14], (__m512i) m[15]);

  a = _mm512_unpacklo_epi64 (r[0], r[1]);
  b = _mm512_unpacklo_epi64 (r[2], r[3]);
  c = _mm512_unpacklo_epi64 (r[4], r[5]);
  d = _mm512_unpacklo_epi64 (r[6], r[7]);
  x = _mm512_permutex2var_epi64 (a, pm1, b);
  y = _mm512_permutex2var_epi64 (c, pm1, d);
  m[0] = (u32x16) _mm512_permutex2var_epi64 (x, pm3, y);
  m[8] = (u32x16) _mm512_permutex2var_epi64 (x, pm4, y);
  x = _mm512_permutex2var_epi64 (a, pm2, b);
  y = _mm512_permutex2var_epi64 (c, pm2, d);
  m[4] = (u32x16) _mm512_permutex2var_epi64 (x, pm3, y);
  m[12] = (u32x16) _mm512_permutex2var_epi64 (x, pm4, y);

  a = _mm512_unpacklo_epi64 (r[8], r[9]);
  b = _mm512_unpacklo_epi64 (r[10], r[11]);
  c = _mm512_unpacklo_epi64 (r[12], r[13]);
  d = _mm512_unpacklo_epi64 (r[14], r[15]);
  x = _mm512_permutex2var_epi64 (a, pm1, b);
  y = _mm512_permutex2var_epi64 (c, pm1, d);
  m[2] = (u32x16) _mm512_permutex2var_epi64 (x, pm3, y);
  m[10] = (u32x16) _mm512_permutex2var_epi64 (x, pm4, y);
  x = _mm512_permutex2var_epi64 (a, pm2, b);
  y = _mm512_permutex2var_epi64 (c, pm2, d);
  m[6] = (u32x16) _mm512_permutex2var_epi64 (x, pm3, y);
  m[14] = (u32x16) _mm512_permutex2var_epi64 (x, pm4, y);

  a = _mm512_unpackhi_epi64 (r[0], r[1]);
  b = _mm512_unpackhi_epi64 (r[2], r[3]);
  c = _mm512_unpackhi_epi64 (r[4], r[5]);
  d = _mm512_unpackhi_epi64 (r[6], r[7]);
  x = _mm512_permutex2var_epi64 (a, pm1, b);
  y = _mm512_permutex2var_epi64 (c, pm1, d);
  m[1] = (u32x16) _mm512_permutex2var_epi64 (x, pm3, y);
  m[9] = (u32x16) _mm512_permutex2var_epi64 (x, pm4, y);
  x = _mm512_permutex2var_epi64 (a, pm2, b);
  y = _mm512_permutex2var_epi64 (c, pm2, d);
  m[5] = (u32x16) _mm512_permutex2var_epi64 (x, pm3, y);
  m[13] = (u32x16) _mm512_permutex2var_epi64 (x, pm4, y);

  a = _mm512_unpackhi_epi64 (r[8], r[9]);
  b = _mm512_unpackhi_epi64 (r[10], r[11]);
  c = _mm512_unpackhi_epi64 (r[12], r[13]);
  d = _mm512_unpackhi_epi64 (r[14], r[15]);
  x = _mm512_permutex2var_epi64 (a, pm1, b);
  y = _mm512_permutex2var_epi64 (c, pm1, d);
  m[3] = (u32x16) _mm512_permutex2var_epi64 (x, pm3, y);
  m[11] = (u32x16) _mm512_permutex2var_epi64 (x, pm4, y);
  x = _mm512_permutex2var_epi64 (a, pm2, b);
  y = _mm512_permutex2var_epi64 (c, pm2, d);
  m[7] = (u32x16) _mm512_permutex2var_epi64 (x, pm3, y);
  m[15] = (u32x16) _mm512_permutex2var_epi64 (x, pm4, y);
}



static_always_inline void
u64x8_transpose (u64x8 m[8])
{
  __m512i r[8], x, y;

  /* *INDENT-OFF* */
  __m512i pm1 = (__m512i) (u64x8) { 0, 1, 8, 9, 4, 5, 12, 13};
  __m512i pm2 = (__m512i) (u64x8) { 2, 3, 10, 11, 6, 7, 14, 15};
  __m512i pm3 = (__m512i) (u64x8) { 0, 1, 2, 3, 8, 9, 10, 11};
  __m512i pm4 = (__m512i) (u64x8) { 4, 5, 6, 7, 12, 13, 14, 15};
  /* *INDENT-ON* */

  r[0] = _mm512_unpacklo_epi64 ((__m512i) m[0], (__m512i) m[1]);
  r[1] = _mm512_unpacklo_epi64 ((__m512i) m[2], (__m512i) m[3]);
  r[2] = _mm512_unpacklo_epi64 ((__m512i) m[4], (__m512i) m[5]);
  r[3] = _mm512_unpacklo_epi64 ((__m512i) m[6], (__m512i) m[7]);
  r[4] = _mm512_unpackhi_epi64 ((__m512i) m[0], (__m512i) m[1]);
  r[5] = _mm512_unpackhi_epi64 ((__m512i) m[2], (__m512i) m[3]);
  r[6] = _mm512_unpackhi_epi64 ((__m512i) m[4], (__m512i) m[5]);
  r[7] = _mm512_unpackhi_epi64 ((__m512i) m[6], (__m512i) m[7]);

  x = _mm512_permutex2var_epi64 (r[0], pm1, r[1]);
  y = _mm512_permutex2var_epi64 (r[2], pm1, r[3]);
  m[0] = (u64x8) _mm512_permutex2var_epi64 (x, pm3, y);
  m[4] = (u64x8) _mm512_permutex2var_epi64 (x, pm4, y);
  x = _mm512_permutex2var_epi64 (r[0], pm2, r[1]);
  y = _mm512_permutex2var_epi64 (r[2], pm2, r[3]);
  m[2] = (u64x8) _mm512_permutex2var_epi64 (x, pm3, y);
  m[6] = (u64x8) _mm512_permutex2var_epi64 (x, pm4, y);

  x = _mm512_permutex2var_epi64 (r[4], pm1, r[5]);
  y = _mm512_permutex2var_epi64 (r[6], pm1, r[7]);
  m[1] = (u64x8) _mm512_permutex2var_epi64 (x, pm3, y);
  m[5] = (u64x8) _mm512_permutex2var_epi64 (x, pm4, y);
  x = _mm512_permutex2var_epi64 (r[4], pm2, r[5]);
  y = _mm512_permutex2var_epi64 (r[6], pm2, r[7]);
  m[3] = (u64x8) _mm512_permutex2var_epi64 (x, pm3, y);
  m[7] = (u64x8) _mm512_permutex2var_epi64 (x, pm4, y);
}

#endif /* included_vector_avx512_h */
/*
 * fd.io coding-style-patch-verification: ON
 *
 * Local Variables:
 * eval: (c-set-style "gnu")
 * End:
 */
