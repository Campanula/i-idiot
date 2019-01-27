#ifndef POINT_H
#define POINT_H

struct Point {
    const void * class;
    int x, y;
};

extern const void * Point;
void move(void * point, int dx, int dy);

#define	x(p)    (((const struct Point *)(p)) -> x)
#define	y(p)    (((const struct Point *)(p)) -> y)

#endif

