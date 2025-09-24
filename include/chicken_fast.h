#pragma once
#include "chicken.h"
#include "config.h"

extern Atlas  atlasChickenFast;


class ChickenFast : public Chicken
{
public:
    ChickenFast()
    {
        animationRun.addFrame(&atlasChickenFast);
        speedRun = Config::getInstance()->get("chicken.speed.fast");
    }
    ~ChickenFast() = default;
};
