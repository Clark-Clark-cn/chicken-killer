#pragma once 
#include "chicken.h"

extern Atlas  atlasChickenMedium;

class ChickenMedium : public Chicken
{
public:
    ChickenMedium()
    {
        animationRun.addFrame(&atlasChickenMedium);
        speedRun = 50.0f;
    }
    ~ChickenMedium() = default;
};