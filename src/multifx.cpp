// Created by Bradly Landucci

#include <usermodfx.h>
#include "fx_api.h"

// Initializing Params //
static float fxamt = 0.5f;
static int type = 01.f;
static int bit_rate = 16;

// Simple waveshaping algorithm //
// x' = 3/2x - 1/2(x^3)
// https://www.musicdsp.org/en/latest/Effects/114-waveshaper-simple-description.html
float __fast_inline waveshape(float in) 
{
    return 1.5f * in - 0.5f * in *in * in;
}

// Simple bit reduction algorithm //
// x' = -1,                                                           {x = -1}
// x' = roundup(bits * input) * ( 1 / bits) <-- Reciprorocal          { Else }
float __fast_inline bitreduc(float in, float bits)
{
  bits = fasterpow2f(bits - 1.0f);
  return in == -1? -1 : ceil(bits * in) * (1 / bits);
}

// Initializing Platform //
void MODFX_INIT(uint32_t platform, uint32_t api)
{
}

// Main DSP Process Block //
// Sub values to be ignored, only used for prologue
void MODFX_PROCESS(const float *xn, float *yn,
                   const float *sub_xn, float *sub_yn,
                   uint32_t frames)
{
  float base_main;

  // Effect processing loop //
  // For double frames (AKA samples cause each frame = sample pair)
  for (int i=0;i<frames*2;i++)
  {
    switch(type)
    {
      // Bit Crushing Algorithm //
      case 0:
        *yn++ = bitreduc(*xn++, bit_rate); 
        break;
      case 1:
        *yn++ = *xn++;        
        break;
      case 2:
        *yn++ = *xn++;
        break;
      // Waveshaping algorithm // 
      case 3:
        base_main = *xn++ * ((fxamt * 10.0f) + 1.f);
        *yn++ = waveshape(base_main);
        break;
    }

  }
}

// Param Controls //
void MODFX_PARAM(uint8_t index, int32_t value)
{
  //Convert fixed-point q31 format to float
  const float valf = q31_to_f32(value);
  switch (index)
  {
    case 0:
      fxamt = valf;

      if (valf < 0.25) 
      {
        bit_rate = 8;
      } 
      else if (valf < 0.5) 
      {
        bit_rate = 16;
      } 
      else if (valf < 0.75) 
      {
        bit_rate = 32;
      } 
      else 
      {
        bit_rate = 64;
      }
      break;

    case 1:
      if (valf < 0.25) 
      {
        type = 0;
      } 
      else if (valf < 0.5) 
      {
        type = 1; 
      } 
      else if (valf < 0.75) 
      {
        type = 2; 
      } 
      else 
      {
        type = 3; // Waveshaper
      }
      break;
    default:
      break;
  }
}