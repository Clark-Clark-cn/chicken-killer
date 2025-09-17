#pragma once
#include"camera.h"
#include"vector2.h"
#include"animation.h"

#include<SDL_mixer.h>

extern Atlas  atlasExplosion;
extern Mix_Chunk* soundExplosion;

class Chicken
{
public:
    Chicken()
    {
        animationRun.setLoop(true);
        animationRun.setInterval(0.1f);

        animationExplosion.setLoop(false);
        animationExplosion.setInterval(0.08f);
        animationExplosion.addFrame(&atlasExplosion);
        animationExplosion.setOnFinished([&]()
            {
                isValid = false;
            });
        position.x = 40.0f + (rand() % 1200);
        position.y = -50;
    }
    ~Chicken() = default;

    const Vector2& getPosition() const
    {
        return position;
    }

    void onUpdate(float delta)
    {
        if (isAlive) position.y += speedRun * delta;
        animationCurrent = (isAlive ? &animationRun : &animationExplosion);
        animationCurrent->setPosition(position);
        animationCurrent->onUpdate(delta);
    }
    void onRender(const Camera& camera) const
    {
        animationCurrent->onRender(camera);
    }
    void onHurt()
    {
        isAlive = false;

        Mix_PlayChannel(-1, soundExplosion, 0);
    }

    void makeInvalid()
    {
        isValid = false;
    }
    bool checkAlive() const
    {
        return isAlive;
    }
    bool canRemove() const
    {
        return !isValid;
    }
protected:
    float speedRun = 10.0f;
    Animation animationRun;
private:
    Vector2 position;
    Animation animationExplosion;
    Animation* animationCurrent = &animationRun;

    bool isAlive = true;
    bool isValid = true;
};