/********************************************************************
 *                                                                  *
 * THIS FILE IS PART OF THE OggVorbis SOFTWARE CODEC SOURCE CODE.   *
 * USE, DISTRIBUTION AND REPRODUCTION OF THIS LIBRARY SOURCE IS     *
 * GOVERNED BY A BSD-STYLE SOURCE LICENSE INCLUDED WITH THIS SOURCE *
 * IN 'COPYING'. PLEASE READ THESE TERMS BEFORE DISTRIBUTING.       *
 *                                                                  *
 * THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2002             *
 * by the XIPHOPHORUS Company http://www.xiph.org/                  *
 *                                                                  *
 ********************************************************************

 function: key psychoacoustic settings for 44.1/48kHz
 last mod: $Id: psych_44.h,v 1.29 2003/09/02 07:32:17 xiphmont Exp $

 ********************************************************************/


/* preecho trigger settings *****************************************/

static vorbis_info_psy_global _psy_global_44[5]={

  {8,   /* lines per eighth octave */
   {20.f,14.f,12.f,12.f,12.f,12.f,12.f},
   {-60.f,-30.f,-40.f,-40.f,-40.f,-40.f,-40.f}, 2,-75.f,
   -6.f,
   {99.},{{99.},{99.}},{0},{0},{{0.},{0.}}
  },
  {8,   /* lines per eighth octave */
   {14.f,10.f,10.f,10.f,10.f,10.f,10.f},
   {-40.f,-30.f,-25.f,-25.f,-25.f,-25.f,-25.f}, 2,-80.f,
   -6.f,
   {99.},{{99.},{99.}},{0},{0},{{0.},{0.}}
  },
  {8,   /* lines per eighth octave */
   {12.f,10.f,10.f,10.f,10.f,10.f,10.f},
   {-20.f,-20.f,-15.f,-15.f,-15.f,-15.f,-15.f}, 0,-80.f,
   -6.f,
   {99.},{{99.},{99.}},{0},{0},{{0.},{0.}}
  },
  {8,   /* lines per eighth octave */
   {10.f,8.f,8.f,8.f,8.f,8.f,8.f},
   {-20.f,-15.f,-12.f,-12.f,-12.f,-12.f,-12.f}, 0,-80.f,
   -6.f,
   {99.},{{99.},{99.}},{0},{0},{{0.},{0.}}
  },
  {8,   /* lines per eighth octave */
   {10.f,6.f,6.f,6.f,6.f,6.f,6.f},
   {-15.f,-15.f,-12.f,-12.f,-12.f,-12.f,-12.f}, 0,-85.f,
   -6.f,
   {99.},{{99.},{99.}},{0},{0},{{0.},{0.}}
  },
};

/* noise compander lookups * low, mid, high quality ****************/
static compandblock _psy_compand_44[6]={
  /* sub-mode Z short */
  {{
    0, 1, 2, 3, 4, 5, 6,  7,     /* 7dB */
    8, 9,10,11,12,13,14, 15,     /* 15dB */
    16,17,18,19,20,21,22, 23,     /* 23dB */
    24,25,26,27,28,29,30, 31,     /* 31dB */
    32,33,34,35,36,37,38, 39,     /* 39dB */
  }},
  /* mode_Z nominal short*/
  {{
     0, 1, 2, 3, 4, 5, 6,  6,     /* 7dB */
     7, 7, 7, 7, 6, 6, 6,  7,     /* 15dB */
     7, 8, 9,10,11,12,13, 14,     /* 23dB */
    15,16,17,17,17,18,18, 19,     /* 31dB */
    19,19,20,21,22,23,24, 25,     /* 39dB */
  }},
  /* mode A short */
  {{
    0, 1, 2, 3, 4, 5, 5,  5,     /* 7dB */
    6, 6, 6, 5, 4, 4, 4,  4,     /* 15dB */
    4, 4, 5, 5, 5, 6, 6,  6,     /* 23dB */
    7, 7, 7, 8, 8, 8, 9, 10,     /* 31dB */
    11,12,13,14,15,16,17, 18,     /* 39dB */
  }},
  /* sub-mode Z long */
  {{
     0, 1, 2, 3, 4, 5, 6,  7,     /* 7dB */
     8, 9,10,11,12,13,14, 15,     /* 15dB */
    16,17,18,19,20,21,22, 23,     /* 23dB */
    24,25,26,27,28,29,30, 31,     /* 31dB */
    32,33,34,35,36,37,38, 39,     /* 39dB */
  }},
  /* mode_Z nominal long */
  {{
    0, 1, 2, 3, 4, 5, 6,  7,     /* 7dB */
    8, 9,10,11,12,12,13, 13,     /* 15dB */
    13,14,14,14,15,15,15, 15,     /* 23dB */
    16,16,17,17,17,18,18, 19,     /* 31dB */
    19,19,20,21,22,23,24, 25,     /* 39dB */
  }},
  /* mode A long */
  {{
    0, 1, 2, 3, 4, 5, 6,  7,     /* 7dB */
    8, 8, 7, 6, 5, 4, 4,  4,     /* 15dB */
    4, 4, 5, 5, 5, 6, 6,  6,     /* 23dB */
    7, 7, 7, 8, 8, 8, 9, 10,     /* 31dB */
    11,12,13,14,15,16,17, 18,     /* 39dB */
  }}
};

/* tonal masking curve level adjustments *************************/
static vp_adjblock _vp_tonemask_adj_longblock[11]={
  /* adjust for mode zero */
  /* 63     125     250     500     1     2     4     8    16 */
  {{-15,-15,-15,-15,-10, -8, -4,-2, 0, 0, 0,10, 0, 0, 0, 0, 0}}, /* 0 */
  {{-15,-15,-15,-15,-15,-12,-10,-8, 0, 0, 0, 5, 0, 0, 0, 0, 0}}, /* 1 */
  {{-15,-15,-15,-15,-15,-12,-10,-8, 0, 0, 0, 0, 0, 0, 0, 0, 0}}, /* 2 */
  {{-15,-15,-15,-15,-15,-12,-10,-8, 0, 0, 0, 0, 0, 0, 0, 0, 0}}, /* 3 */
  {{-15,-15,-15,-15,-15,-12,-10,-8, 0, 0, 0, 0, 0, 0, 0, 0, 0}}, /* 4 */
  {{-15,-15,-15,-15,-15,-12,-10,-8, 0, 0, 0, 0, 0, 0, 0, 0, 0}}, /* 5 */
  {{-15,-15,-15,-15,-15,-12,-10,-8, 0, 0, 0, 0, 0, 0, 0, 0, 0}}, /* 6 */
  {{-15,-15,-15,-15,-15,-12,-10,-8, 0, 0, 0, 0, 0, 0, 0, 0, 0}}, /* 7 */
  {{-15,-15,-15,-15,-15,-12,-10,-8, 0, 0, 0, 0, 0, 0, 0, 0, 0}}, /* 8 */
  {{-15,-15,-15,-15,-15,-12,-10,-8, 0, 0, 0, 0, 0, 0, 0, 0, 0}}, /* 9 */
  {{-15,-15,-15,-15,-15,-12,-10,-8, 0, 0, 0, 0, 0, 0, 0, 0, 0}}, /* 10 */
};
static vp_adjblock _vp_tonemask_adj_otherblock[11]={
  /* adjust for mode zero */
  /* 63     125     250     500       1     2     4     8    16 */
  {{-20,-20,-20,-20,-14,-12,-10, -8, -4, 0, 0,10, 0, 0, 0, 0, 0}}, /* 0 */
  {{-20,-20,-20,-20,-20,-18,-16,-14,-10, 0, 0, 5, 0, 0, 0, 0, 0}}, /* 1 */
  {{-20,-20,-20,-20,-20,-18,-16,-14,-10, 0, 0, 0, 0, 0, 0, 0, 0}}, /* 2 */
  {{-20,-20,-20,-20,-20,-18,-16,-14,-10, 0, 0, 0, 0, 0, 0, 0, 0}}, /* 3 */
  {{-20,-20,-20,-20,-20,-18,-16,-14,-10, 0, 0, 0, 0, 0, 0, 0, 0}}, /* 4 */
  {{-20,-20,-20,-20,-20,-18,-16,-14,-10, 0, 0, 0, 0, 0, 0, 0, 0}}, /* 5 */
  {{-20,-20,-20,-20,-20,-18,-16,-14,-10, 0, 0, 0, 0, 0, 0, 0, 0}}, /* 6 */
  {{-20,-20,-20,-20,-20,-18,-16,-14,-10, 0, 0, 0, 0, 0, 0, 0, 0}}, /* 7 */
  {{-20,-20,-20,-20,-20,-18,-16,-14,-10, 0, 0, 0, 0, 0, 0, 0, 0}}, /* 8 */
  {{-20,-20,-20,-20,-20,-18,-16,-14,-10, 0, 0, 0, 0, 0, 0, 0, 0}}, /* 9 */
  {{-20,-20,-20,-20,-20,-18,-16,-14,-10, 0, 0, 0, 0, 0, 0, 0, 0}}, /* 10 */
};

static noise3 _psy_noisebias_trans_low[2]={
  /*  63     125     250     500      1k       2k      4k      8k     16k*/
  /* 0 */
  {{{-10,-10,-10,-10,-10, -4,  0,  0,  4,  8,  8,  8,  8, 10, 12, 14, 20},
    {-30,-30,-30,-30,-26,-20,-16, -8, -6, -6, -2,  2,  2,  4,  8,  8, 15},
    {-30,-30,-30,-30,-30,-24,-20,-14,-10, -6, -8, -8, -6, -6, -6, -4, -2}}},
  /* 1 */
  {{{-15,-15,-15,-15,-15,-10, -5,  0,  2,  2,  6,  6,  6,  8, 10, 12, 15},
    {-30,-30,-30,-30,-26,-22,-20,-14,-10, -4, -2,  0,  0,  0,  2,  4,  10},
    {-30,-30,-30,-30,-26,-22,-20,-14,-10, -6, -6, -6, -6, -4, -4, -4,  -2}}},
};
static noise3 _psy_noisebias_long_low[2]={
    /*63     125     250     500      1k       2k      4k      8k     16k*/
  /* 0 */
  {{{-10,-10,-10,-10,-10, -4,  0,  0,  0,  6,  6,  6,  6, 10, 10, 12,  20},
    {-20,-20,-20,-20,-20,-20,-10, -2,  0,  0,  0,  0,  0,  2,  4,  6,  15},
    {-20,-20,-20,-20,-20,-20,-20,-10, -6, -6, -6, -6, -6, -4, -4, -4, -2}}},
  /* 1 */
  {{{-10,-10,-10,-10,-10,-10, -8, -8,  0,  2,  4,  4,  5,  5,  5,  8,  10},
    {-20,-20,-20,-20,-20,-20,-20,-14, -8, -2,  0,  0,  0,  0,  2,  4,  10},
    {-20,-20,-20,-20,-20,-20,-20,-14, -6, -6, -6, -6, -6, -4, -4, -4, -2}}},
};

static noise3 _psy_noisebias_trans[11]={
  /*  63     125     250     500      1k       2k      4k      8k     16k*/
  /* 0 */
  {{{-15,-15,-15,-15,-15,-12,-10, -8,  0,  2,  4,  4,  5,  5,  5,  8,  10},
    {-30,-30,-30,-30,-26,-22,-20,-14, -8, -4,  0,  0,  0,  0,  2,  4,  10},
    {-30,-30,-30,-30,-26,-22,-20,-14,-10, -6, -6, -6, -6, -4, -4, -4,  -2}}},
  /* 1 */
  {{{-15,-15,-15,-15,-15,-12,-10, -8,  0,  2,  4,  4,  5,  5,  5,  8,  10},
    {-30,-30,-30,-30,-26,-22,-20,-14,-10, -4, -2, -2, -2, -2,  0,  2,  8},
    {-30,-30,-30,-30,-26,-22,-20,-14,-10, -8, -8, -8, -8, -6, -6, -6, -4}}},
  /* 2 */
  {{{-15,-15,-15,-15,-15,-12,-10, -8,  0,  2,  2,  2,  4,  4,  5,  6,  10},
    {-30,-30,-30,-30,-26,-22,-20,-14,-10, -4, -2, -2, -2, -2,  0,  2,  6},
    {-30,-30,-30,-30,-26,-22,-20,-14,-10,-10,-10,-10,-10, -8, -8, -8, -4}}},
  /* 3 */
  {{{-15,-15,-15,-15,-15,-12,-10, -8,  0,  2,  2,  2,  4,  4,  4,  5,  8},
    {-30,-30,-30,-30,-26,-22,-20,-14,-10, -4, -3, -3, -3, -3, -1,  1,  6},
    {-30,-30,-30,-30,-26,-22,-20,-14,-10,-10,-10,-10,-10, -8, -8, -8, -4}}},
  /* 4 */
  {{{-20,-20,-20,-20,-20,-18,-14, -8, -1,  1,  1,  1,  2,  3,  3,  4,  7},
    {-30,-30,-30,-30,-26,-22,-20,-14,-10, -4, -3, -3, -3, -3, -1,  1,  5},
    {-30,-30,-30,-30,-26,-22,-20,-14,-10,-10,-10,-10,-10, -8, -8, -8, -4}}},
  /* 5 */
  {{{-24,-24,-24,-24,-20,-18,-14, -8, -1,  1,  1,  1,  2,  3,  3,  4,  7},
    {-32,-32,-32,-32,-28,-24,-22,-16,-12, -6, -4, -4, -4, -4, -2, -1,  2},
    {-34,-34,-34,-34,-30,-24,-24,-18,-14,-12,-12,-12,-12,-10,-10, -9, -5}}},
  /* 6 */
  {{{-24,-24,-24,-24,-20,-18,-14, -8, -1,  1,  1,  1,  2,  3,  3,  4,  7},
    {-32,-32,-32,-32,-28,-24,-24,-18,-14, -8, -6, -6, -6, -6, -4, -2,  1},
    {-34,-34,-34,-34,-30,-26,-24,-18,-17,-15,-15,-15,-15,-13,-13,-12, -8}}},
  /* 7 */
  {{{-24,-24,-24,-24,-20,-18,-14, -8, -1,  1,  1,  1,  2,  3,  3,  4, 7},
    {-32,-32,-32,-32,-28,-24,-24,-18,-14,-12,-10, -8, -8, -8, -6, -4,  0},
    {-34,-34,-34,-34,-30,-26,-26,-24,-22,-19,-19,-19,-19,-18,-17,-16,-12}}},
  /* 8 */
  {{{-24,-24,-24,-24,-22,-20,-15,-10, -8, -2,  0,  0,  0,  1,  2,  3,  7},
    {-36,-36,-36,-36,-30,-30,-30,-24,-18,-14,-12,-10,-10,-10, -8, -6, -2},
    {-36,-36,-36,-36,-34,-30,-28,-26,-24,-24,-24,-24,-24,-24,-24,-20,-16}}},
  /* 9 */
  {{{-28,-28,-28,-28,-28,-28,-28,-20,-14, -8, -4, -4, -4, -4, -4, -2,  2},
    {-36,-36,-36,-36,-34,-32,-32,-28,-20,-16,-16,-16,-16,-14,-12,-10, -7},
    {-40,-40,-40,-40,-40,-40,-40,-32,-30,-30,-30,-30,-30,-30,-30,-24,-20}}},
  /* 10 */
  {{{-30,-30,-30,-30,-30,-30,-30,-28,-20,-14,-14,-14,-14,-14,-14,-12,-10},
    {-40,-40,-40,-40,-40,-40,-40,-40,-35,-30,-30,-30,-30,-30,-30,-30,-20},
    {-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40}}},
};

static noise3 _psy_noisebias_long[11]={
    /*63     125     250     500      1k       2k      4k      8k     16k*/
  /* 0 */
  {{{-10,-10,-10,-10,-10,-10, -8,  2,  2,  2,  4,  4,  5,  5,  5,  8,  10},
    {-20,-20,-20,-20,-20,-20,-20,-14, -6,  0,  0,  0,  0,  0,  2,  4,  10},
    {-20,-20,-20,-20,-20,-20,-20,-14, -8, -6, -6, -6, -6, -4, -4, -4, -2}}},
  /* 1 */
  {{{-10,-10,-10,-10,-10,-10, -8, -4,  0,  2,  4,  4,  5,  5,  5,  8,  10},
    {-20,-20,-20,-20,-20,-20,-20,-14,-10, -4, -2, -2, -2, -2,  0,  2,  8},
    {-20,-20,-20,-20,-20,-20,-20,-14,-10, -8, -8, -8, -8, -6, -6, -6, -4}}},
  /* 2 */
  {{{-10,-10,-10,-10,-10,-10,-10, -8,  0,  2,  2,  2,  4,  4,  5,  6,  10},
    {-20,-20,-20,-20,-20,-20,-20,-14,-10, -4, -2, -2, -2, -2,  0,  2,  6},
    {-20,-20,-20,-20,-20,-20,-20,-14,-10,-10,-10,-10,-10, -8, -8, -8, -4}}},
  /* 3 */
  {{{-10,-10,-10,-10,-10,-10,-10, -8,  0,  2,  2,  2,  4,  4,  4,  5,  8},
    {-20,-20,-20,-20,-20,-20,-20,-14,-10, -4, -3, -3, -3, -3, -1,  1,  6},
    {-20,-20,-20,-20,-20,-20,-20,-14,-10,-10,-10,-10,-10, -8, -8, -8, -4}}},
  /* 4 */
  {{{-15,-15,-15,-15,-15,-15,-15,-10, -4,  1,  1,  1,  2,  3,  3,  4,  7},
    {-20,-20,-20,-20,-20,-20,-20,-14,-10, -4, -3, -3, -3, -3, -1,  1,  5},
    {-20,-20,-20,-20,-20,-20,-20,-14,-10,-10,-10,-10,-10, -8, -8, -8, -4}}},
  /* 5 */
  {{{-15,-15,-15,-15,-15,-15,-15,-10, -4,  1,  1,  1,  2,  3,  3,  4,  7},
    {-22,-22,-22,-22,-22,-22,-22,-16,-12, -6, -4, -4, -4, -4, -2, -1,  2},
    {-24,-24,-24,-24,-24,-24,-24,-18,-14,-12,-12,-12,-12,-10,-10, -9, -5}}},
  /* 6 */
  {{{-15,-15,-15,-15,-15,-15,-15,-10, -4,  1,  1,  1,  2,  3,  3,  4,  7},
    {-24,-24,-24,-24,-24,-24,-24,-18,-14, -8, -6, -6, -6, -6, -4, -2,  1},
    {-26,-26,-26,-26,-26,-26,-26,-18,-16,-15,-15,-15,-15,-13,-13,-12, -8}}},
  /* 7 */
  {{{-15,-15,-15,-15,-15,-15,-15,-10, -4,  1,  1,  1,  2,  3,  3,  4,  7},
    {-24,-24,-24,-24,-24,-24,-24,-18,-14,-10, -8, -8, -8, -8, -6, -4,  0},
    {-26,-26,-26,-26,-26,-26,-26,-22,-20,-19,-19,-19,-19,-18,-17,-16,-12}}},
  /* 8 */
  {{{-15,-15,-15,-15,-15,-15,-15,-10, -4,  0,  0,  0,  0,  1,  2,  3,  7},
    {-26,-26,-26,-26,-26,-26,-26,-20,-16,-12,-10,-10,-10,-10, -8, -6, -2},
    {-28,-28,-28,-28,-28,-28,-28,-26,-24,-24,-24,-24,-24,-24,-24,-20,-16}}},
  /* 9 */
  {{{-22,-22,-22,-22,-22,-22,-22,-18,-14, -8, -4, -4, -4, -4, -4, -2,  2},
    {-26,-26,-26,-26,-26,-26,-26,-22,-18,-16,-16,-16,-16,-14,-12,-10, -7},
    {-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-24,-20}}},
  /* 10 */
  {{{-24,-24,-24,-24,-24,-24,-24,-24,-24,-18,-14,-14,-14,-14,-14,-12,-10},
    {-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-20},
    {-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40}}},
};

static noise3 _psy_noisebias_impulse[11]={
  /*  63     125     250     500      1k      2k      4k      8k     16k*/
  /* 0 */
  {{{-10,-10,-10,-10,-10, -4,  0,  0,  4,  4,  8,  8,  8, 10, 12, 14, 20},
    {-30,-30,-30,-30,-26,-22,-20,-14, -6, -2,  0,  0,  0,  0,  2,  4,  10},
    {-30,-30,-30,-30,-30,-24,-20,-14,-10, -6, -8, -8, -6, -6, -6, -4, -2}}},
  /* 1 */
  {{{-12,-12,-12,-12,-12, -8, -6, -4,  0,  4,  4,  4,  4, 10, 12, 14, 20},
    {-30,-30,-30,-30,-26,-22,-20,-14,-10, -6, -4, -4, -2, -2, -2, -2,  2},
    {-30,-30,-30,-30,-26,-22,-20,-14,-10, -8,-10,-10, -8, -8, -8, -6, -4}}},
  /* 2 */
  {{{-14,-14,-14,-14,-14,-10, -8, -6, -2,  2,  2,  2,  2,  8, 10, 10, 16},
    {-30,-30,-30,-30,-26,-22,-20,-14,-10, -6, -6, -6, -4, -4, -4, -2,  0},
    {-30,-30,-30,-30,-26,-22,-20,-14,-10,-10,-10,-10,-10,-10,-10, -8, -4}}},
  /* 3 */
  {{{-14,-14,-14,-14,-14,-10, -8, -6, -2,  2,  2,  2,  2,  6,  8,  8, 14},
    {-30,-30,-30,-30,-26,-22,-20,-14,-10, -6, -6, -6, -4, -4, -4, -2,  0},
    {-30,-30,-30,-30,-26,-22,-20,-14,-10,-10,-10,-10,-10,-10,-10, -8, -4}}},
  /* 4 */
  {{{-16,-16,-16,-16,-16,-12,-10, -6, -2,  0,  0,  0,  0,  4,  6,  6, 12},
    {-30,-30,-30,-30,-26,-22,-20,-14,-10, -6, -6, -6, -4, -4, -4, -2,  0},
    {-30,-30,-30,-30,-26,-22,-20,-14,-10,-10,-10,-10,-10,-10,-10, -8, -4}}},
  /* 5 */
  {{{-20,-20,-20,-20,-20,-18,-14,-10, -4,  0,  0,  0,  0,  4,  4,  6, 11},
    {-32,-32,-32,-32,-28,-24,-22,-16,-10, -6, -8, -8, -6, -6, -6, -4, -2},
    {-34,-34,-34,-34,-30,-26,-24,-18,-14,-12,-12,-12,-12,-12,-10, -9, -5}}},
  /* 6 */
  {{{-20,-20,-20,-20,-20,-18,-14,-10, -4,  0,  0,  0,  0,  4,  4,  6, 11},
    {-34,-34,-34,-34,-30,-30,-24,-20,-12,-12,-14,-14,-10, -9, -8, -6, -4},
    {-34,-34,-34,-34,-34,-30,-26,-20,-16,-15,-15,-15,-15,-15,-13,-12, -8}}},
  /* 7 */
  {{{-22,-22,-22,-22,-22,-20,-14,-10, -6,  0,  0,  0,  0,  4,  4,  6, 11},
    {-34,-34,-34,-34,-30,-30,-24,-20,-14,-14,-16,-16,-14,-12,-10,-10,-10},
    {-34,-34,-34,-34,-32,-32,-30,-24,-20,-19,-19,-19,-19,-19,-17,-16,-12}}},
  /* 8 */
  {{{-24,-24,-24,-24,-24,-22,-14,-10, -6, -1, -1, -1, -1,  3,  3,  5, 10},
    {-34,-34,-34,-34,-30,-30,-30,-24,-20,-20,-20,-20,-20,-18,-16,-16,-14},
    {-36,-36,-36,-36,-36,-34,-28,-24,-24,-24,-24,-24,-24,-24,-24,-20,-16}}},
  /* 9 */
  {{{-28,-28,-28,-28,-28,-28,-28,-20,-14, -8, -4, -4, -4, -4, -4, -2,  2},
    {-36,-36,-36,-36,-34,-32,-32,-30,-26,-26,-26,-26,-26,-22,-20,-20,-18},
    {-40,-40,-40,-40,-40,-40,-40,-32,-30,-30,-30,-30,-30,-30,-30,-24,-20}}},
  /* 10 */
  {{{-30,-30,-30,-30,-30,-26,-24,-24,-24,-20,-16,-16,-16,-16,-16,-14,-12},
    {-40,-40,-40,-40,-40,-40,-40,-40,-35,-30,-30,-30,-30,-30,-30,-30,-26},
    {-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40}}},
};


static noise3 _psy_noisebias_padding[11]={
  /*  63     125     250     500      1k       2k      4k      8k     16k*/
  /* 0 */
  {{{-10,-10,-10,-10,-10, -4,  0,  0,  4,  8,  8,  8,  8, 10, 12, 14, 20},
    {-30,-30,-30,-30,-26,-22,-20,-14,-10, -4, -2,  2,  3,  6,  6,  8, 10},
    {-30,-30,-30,-30,-26,-22,-20,-14,-10, -4, -4, -4, -4, -4, -2,  0,  2}}},
  /* 1 */
  {{{-12,-12,-12,-12,-12, -8, -6, -4,  0,  4,  4,  4,  4, 10, 12, 14, 20},
    {-30,-30,-30,-30,-26,-22,-20,-14,-10, -4,  0,  0,  0,  2,  2,  4,  8},
    {-30,-30,-30,-30,-26,-22,-20,-14,-10, -6, -6, -6, -6, -6, -4, -2,  0}}},
  /* 2 */
  {{{-14,-14,-14,-14,-14,-10, -8, -6, -2,  2,  2,  2,  2,  8, 10, 10, 16},
    {-30,-30,-30,-30,-26,-22,-20,-14,-10, -4,  0,  0,  0,  2,  2,  4,  8},
    {-30,-30,-30,-30,-26,-22,-20,-14,-10, -8, -8, -8, -8, -8, -6, -4, -2}}},
  /* 3 */
  {{{-14,-14,-14,-14,-14,-10, -8, -6, -2,  2,  2,  2,  2,  6,  8,  8, 14},
    {-30,-30,-30,-30,-26,-22,-20,-14,-10, -6, -1, -1, -1,  0,  0,  2,  6},
    {-30,-30,-30,-30,-26,-22,-20,-14,-10, -8, -8, -8, -8, -8, -6, -4, -2}}},
  /* 4 */
  {{{-16,-16,-16,-16,-16,-12,-10, -6, -2,  0,  0,  0,  0,  4,  6,  6, 12},
    {-30,-30,-30,-30,-26,-22,-20,-14,-10, -6, -1, -1, -1, -1,  0,  2,  6},
    {-30,-30,-30,-30,-26,-22,-20,-14,-10, -8, -8, -8, -8, -8, -6, -4, -2}}},
  /* 5 */
  {{{-20,-20,-20,-20,-20,-18,-14,-10, -4,  0,  0,  0,  0,  4,  6,  6, 12},
    {-32,-32,-32,-32,-28,-24,-22,-16,-12, -6, -3, -3, -3, -3, -2,  0,  4},
    {-34,-34,-34,-34,-30,-26,-24,-18,-14,-10,-10,-10,-10,-10, -8, -5, -3}}},
  /* 6 */
  {{{-20,-20,-20,-20,-20,-18,-14,-10, -4,  0,  0,  0,  0,  4,  6,  6, 12},
    {-34,-34,-34,-34,-30,-30,-24,-20,-14, -8, -4, -4, -4, -4, -3, -1,  4},
    {-34,-34,-34,-34,-34,-30,-26,-20,-16,-13,-13,-13,-13,-13,-11, -8, -6}}},
  /* 7 */
  {{{-20,-20,-20,-20,-20,-18,-14,-10, -4,  0,  0,  0,  0,  4,  6,  6, 12},
    {-34,-34,-34,-34,-30,-30,-30,-24,-16,-10, -8, -6, -6, -6, -5, -3,  1},
    {-34,-34,-34,-34,-32,-32,-28,-22,-18,-16,-16,-16,-16,-16,-14,-12,-10}}},
  /* 8 */
  {{{-22,-22,-22,-22,-22,-20,-14,-10, -4,  0,  0,  0,  0,  3,  5,  5, 11},
    {-34,-34,-34,-34,-30,-30,-30,-24,-16,-12,-10, -8, -8, -8, -7, -5, -2},
    {-36,-36,-36,-36,-36,-34,-28,-22,-20,-20,-20,-20,-20,-20,-20,-16,-14}}},
  /* 9 */
  {{{-28,-28,-28,-28,-28,-28,-28,-20,-14, -8, -2, -2, -2, -2,  0,  2,  6},
    {-36,-36,-36,-36,-34,-32,-32,-24,-16,-12,-12,-12,-12,-12,-10, -8, -5},
    {-40,-40,-40,-40,-40,-40,-40,-32,-26,-24,-24,-24,-24,-24,-24,-20,-18}}},
  /* 10 */
  {{{-30,-30,-30,-30,-30,-26,-24,-24,-24,-20,-12,-12,-12,-12,-12,-10, -8},
    {-40,-40,-40,-40,-40,-40,-40,-40,-35,-30,-25,-25,-25,-25,-25,-25,-15},
    {-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40,-40}}},
};

static noiseguard _psy_noiseguards_44[4]={
  {3,3,15},
  {3,3,15},
  {10,10,100},
  {10,10,100},
};

static int _psy_tone_suppress[11]={
  -20,-20,-20,-20,-24,-30,-40,-40,-45,-45,-45,
};

static int _psy_tone_0dB[11]={
  90,95,95,95,95,105,105,105,105,105,105,
};

static int _psy_noise_suppress[11]={
 -20,-24,-24,-24,-24,-30,-40,-40,-45,-45,-45,
};

static vorbis_info_psy _psy_info_template={
  -1,
  -140.,-140.,
  /* tonemask att boost/decay,suppr,curves */
  {0.f,0.f,0.f},     0.,0.,    -40.f, {0.},

  /*noisemaskp,supp, low/high window, low/hi guard, minimum */
  1,          -0.f,         .5f, .5f,         0,0,0,
  {{-1},{-1},{-1}},{-1},105.f,

  0,0,-1,-1,0.,
};

/* ath ****************/

static int _psy_ath_floater[11]={
  -100,-100,-100,-100,-100,-105,-105,-105,-105,-110,-120,
};

static int _psy_ath_abs[11]={
  -130,-130,-130,-140,-140,-140,-140,-140,-140,-140,-150,
};

/* stereo setup.  These don't map directly to quality level, there's
   an additional indirection as several of the below may be used in a
   single bitmanaged stream

****************/

/* various stereo possibilities */

/* stereo mode by base quality level */
static adj_stereo _psy_stereo_modes_44_low[2]={
  /*  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14         0  */
  {{  4,  4,  4,  4,  4,  4,  4,  3,  2,  2,  1,  0,  0,  0,  0},
   {  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  5,  4,  3},
   {  1,  2,  3,  4,  4,  4,  4,  4,  4,  5,  6,  7,  8,  8,  8},
   { 12,12.5, 13,13.5, 14,14.5, 15, 99, 99, 99, 99, 99, 99, 99, 99}},
  /*  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14         1  */
  {{  4,  4,  4,  4,  4,  4,  4,  3,  2,  2,  1,  0,  0,  0,  0},
   {  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  5,  4,  3},
   {  1,  2,  3,  4,  5,  5,  6,  6,  6,  6,  6,  7,  8,  8,  8},
   { 12,12.5, 13,13.5, 14,14.5, 15, 99, 99, 99, 99, 99, 99, 99, 99}},
};

static adj_stereo _psy_stereo_modes_44[11]={
  /*  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14         0  */
  {{  4,  4,  4,  4,  4,  4,  4,  3,  2,  2,  1,  0,  0,  0,  0},
   {  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  5,  4,  3},
   {  1,  2,  3,  4,  5,  5,  6,  6,  6,  6,  6,  7,  8,  8,  8},
   { 12,12.5, 13,13.5, 14,14.5, 15, 99, 99, 99, 99, 99, 99, 99, 99}},
  /*  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14         1  */
  {{  3,  3,  3,  3,  3,  3,  3,  3,  2,  1,  0,  0,  0,  0,  0},
   {  8,  8,  8,  8,  6,  6,  5,  5,  5,  5,  5,  5,  5,  4,  3},
   {  1,  2,  3,  4,  4,  5,  6,  6,  6,  6,  6,  8,  8,  8,  8},
   { 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99}},
  /*  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14         2  */
  {{  3,  3,  3,  3,  3,  3,  2,  2,  2,  1,  0,  0,  0,  0,  0},
   {  8,  8,  8,  6,  5,  5,  5,  5,  5,  5,  5,  4,  3,  2,  1},
   {  3,  4,  4,  4,  5,  6,  6,  6,  6,  6,  6,  8,  8,  8,  8},
   { 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99}},
  /*  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14         3  */
  {{  2,  2,  2,  2,  2,  1,  1,  1,  1,  0,  0,  0,  0,  0,  0},
   {  5,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  3,  2,  1},
   {  4,  4,  5,  6,  6,  6,  6,  6,  8,  8, 10, 10, 10, 10, 10},
   { 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99}},
  /*  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14         4  */
  {{  2,  2,  2,  1,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0},
   {  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  3,  3,  2,  1,  0},
   {  6,  6,  6,  8,  8,  8,  8,  8,  8,  8, 10, 10, 10, 10, 10},
   { 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99}},
  /*  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14         5  */
  {{  2,  2,  2,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
   {  3,  3,  3,  3,  3,  2,  2,  2,  2,  2,  2,  0,  0,  0,  0},
   {  6,  6,  8,  8,  8,  8, 10, 10, 10, 10, 10, 10, 10, 10, 10},
   { 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99}},
  /*  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14         6  */
  {{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
   {  3,  3,  3,  2,  2,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0},
   {  8,  8,  8,  8, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
   { 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99}},
  /*  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14         7  */
  {{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
   {  3,  3,  3,  2,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
   {  8,  8,  8,  8, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
   { 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99}},
  /*  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14         8  */
  {{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
   {  2,  2,  2,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
   {  8, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10},
   { 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99}},
  /*  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14         9  */
  {{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
   {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
   {  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4},
   { 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99}},
  /*  0   1   2   3   4   5   6   7   8   9  10  11  12  13  14        10  */
  {{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
   {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
   {  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4},
   { 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99}},
};

/* tone master attenuation by base quality mode and bitrate tweak */

static att3 _psy_tone_masteratt_44_low[2]={
  {{ 34,  20,   8},  0, 1.25}, /* 0 */
  {{ 34,  20,   8}, -2, 1.25}, /* 1 */
};
static att3 _psy_tone_masteratt_44[11]={
  {{ 30,  20,   8}, -2, 1.25}, /* 0 */
  {{ 25,  14,   4},  0,    0}, /* 1 */
  {{ 20,  10,  -2},  0,    0}, /* 2 */
  {{ 20,   9,  -4},  0,    0}, /* 3 */
  {{ 20,   9,  -4},  0,    0}, /* 4 */
  {{ 20,   6,  -6},  0,    0}, /* 5 */
  {{ 20,   3, -10},  0,    0}, /* 6 */
  {{ 18,   1, -14},  0,    0}, /* 7 */
  {{ 18,   0, -16},  0,    0}, /* 8 */
  {{ 18,  -2, -16},  0,    0}, /* 9 */
  {{ 12,  -2, -20},  0,    0}, /* 10 */
};

/* lowpass by mode **************/
static double _psy_lowpass_44_low[2]={
  15.1,15.1,
};
static double _psy_lowpass_44[11]={
  15.1,15.8,16.5,17.9,20.5,48.,999.,999.,999.,999.,999.
};

/* noise normalization **********/

static int _noise_start_short_44[10]={
  16,16,16,16,32,32,9999,9999,9999,9999
};
static int _noise_start_long_44[10]={
  128,128,128,256,512,512,9999,9999,9999,9999
};
static int _noise_part_short_44[10]={
  8,8,8,8,8,8,8,8,8,8
};
static int _noise_part_long_44[10]={
  32,32,32,32,32,32,32,32,32,32
};
static double _noise_thresh_44[10]={
  .2,.2,.3,.4,.5,.5,9999.,9999.,9999.,9999.,
};
static double _noise_thresh_44_2[2]={
  .5,.5,
};


static int _noise_start_short_44_low[2]={
  32,32
};
static int _noise_start_long_44_low[2]={
  256,256
};
