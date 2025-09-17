#pragma once
#include "chicken.h"

extern Atlas  atlasChickenSlow;

class ChickenSlow : public Chicken
{
public:
    ChickenSlow()
    {
        animationRun.addFrame(&atlasChickenSlow);
        speedRun = 30.0f;
    }
    ~ChickenSlow() = default;
};
