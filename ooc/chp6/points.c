#include "Point.h"

int main(int argc, char ** argv) {
    void * p;

    // 初始化 PointClass 和 Point
    initPoint();

    while(* ++ argv) {
        switch (** argv) {
            case 'p':
                // struct Object, x, y
                p = new(Point, 1, 2);
                break;
            default:
                continue;
        }
        draw(p);
        move(p, 10, 20);
        draw(p);
        delete(p);
    }
    return 0;
}

