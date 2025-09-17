#pragma once
#include "chicken.h"

extern Atlas  atlasChickenFast;


class ChickenFast : public Chicken
{
public:
    ChickenFast()
    {
        animationRun.addFrame(&atlasChickenFast);
        speedRun = 80.0f;
    }
    ~ChickenFast() = default;
};
