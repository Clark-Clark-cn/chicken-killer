#pragma once
#include "chicken.h"
#include "config.h"

extern Atlas  atlasChickenSlow;

class ChickenSlow : public Chicken
{
public:
    ChickenSlow()
    {
        animationRun.addFrame(&atlasChickenSlow);
        speedRun = Config::getInstance()->get("chicken.speed.slow");
    }
    ~ChickenSlow() = default;
};
