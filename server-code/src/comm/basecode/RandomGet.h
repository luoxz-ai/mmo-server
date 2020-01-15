#ifndef RANDOMGET_H
#define RANDOMGET_H


#include "BaseType.h"
#include "export_lua.h"
export_lua uint32_t random_uint32();
//产生一个随机数[min,max]
export_lua uint32_t random_uint32_range(uint32_t min, uint32_t max);
//产生一个随机数[min,max], 对于范围[0,2pi]：随机地落下一个棒并且测量其以弧度为单位的角度（假设角度均匀分布）
export_lua float random_float(float min = 0.0f, float max = 1.0f);
//根据几率判断是否命中,扔硬币（p = 0.5）
export_lua bool random_hit(float op);

#endif /* RANDOMGET_H */
