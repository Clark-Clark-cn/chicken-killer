#pragma once 
#include "chicken.h"
#include "config.h"

extern Atlas  atlasChickenMedium;

class ChickenMedium : public Chicken
{
public:
    ChickenMedium()
    {
        animationRun.addFrame(&atlasChickenMedium);
        speedRun = Config::getInstance()->get("chicken.speed.medium");
    }
    ~ChickenMedium() = default;
};