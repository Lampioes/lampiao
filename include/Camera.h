#ifndef CAMERA_H
#define CAMERA_H

#include <box2d/box2d.h>

struct Camera {
    b2Vec2 posicao{0.0f, 0.0f};

    void setPosition(float x, float y) { posicao.Set(x, y); }

    int x() const { return static_cast<int>(posicao.x); }
    int y() const { return static_cast<int>(posicao.y); }
};

#endif
