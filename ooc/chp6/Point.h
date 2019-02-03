#ifndef POINT_H
#define POINT_H

#include "Object.h"

extern const void * Point;

// selector
void draw(const void * self);
void move(void * point, int dx, int dy);

extern const void * PointClass;

// 初始化类方法
void initPoint(void);

#endif

