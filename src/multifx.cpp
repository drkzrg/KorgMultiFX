// Created by Bradly Landucci

#include <usermodfx.h>
#include "fx_api.h"

// Initializing Params //
static float fxamt = 0.5f;
static int type = 01.f;

// Simple waveshaping algorithm //
// x' = 3/2x - 1/2(x^3)
// https://www.musicdsp.org/en/latest/Effects/114-waveshaper-simple-description.html
float __fast_inline waveshape(float in) 
{
    return 1.5f * in - 0.5f * in *in * in;
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
    float xn_cur = *xn++;

    // Waveshaping algorithm // 
    base_main = (xn_cur * ((fxamt * 10.0f) + 1.f));
    switch(type)
    {
      case 0:
        *yn++ = base_main;  
        break;
      case 1:
        *yn++ = base_main;        
        break;
      case 2:
        *yn++ = base_main;
        break;
      case 3:
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
      break;
    case 1:
      if (valf < 0.25) 
      {
        type = 0; //Soft
      } 
      else if (valf < 0.5) 
      {
        type = 1; //Hard
      } 
      else if (valf < 0.75) 
      {
        type = 2; //Wrap
      } 
      else 
      {
        type = 3; //..
      }
      break;
    default:
      break;
  }
}