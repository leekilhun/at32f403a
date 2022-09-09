/*
 * ws2812.h
 *
 *  Created on: 2020. 12. 23.
 *      Author: baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_WS2812_H_
#define SRC_COMMON_HW_INCLUDE_WS2812_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"


#ifdef _USE_HW_WS2812

#define WS2812_MAX_CH         HW_WS2812_MAX_CH



#define WS2812_RGB(r,g,b)    ((uint32_t)(b)) | ((uint32_t)(r)<<8) | ((uint32_t)(g)<<16)


typedef struct
{
  uint8_t b;
  uint8_t r;
  uint8_t g;
  uint8_t w;
} ws_pixel_t;

typedef union
{
  ws_pixel_t rgb;
  uint32_t data;
} ws_color_t;


bool ws2812Init(void);
bool ws2812SetColor(uint8_t ch, uint32_t rgb);
bool ws2812Load(void);


#endif


#ifdef __cplusplus
}
#endif

#endif /* SRC_COMMON_HW_INCLUDE_BUTTON_H_ */