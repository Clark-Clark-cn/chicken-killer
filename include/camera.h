#pragma once
#include "timer.h"

#include "vector2.h"
#include <SDL.h>

class Camera
{
public:
    Camera(SDL_Renderer* renderer)
    {
        this->renderer = renderer;

        timerShake.setOneShot(true);
        timerShake.setOnTimeout([&]()
            {
                isShaking = false;reset();
            });
    }
    ~Camera() = default;
    const Vector2& getPosition() const
    {
        return position;
    }

    void reset()
    {
        position.x = 0;
        position.y = 0;
    }

    void onUpdate(float delta)
    {
        timerShake.onUpdate(delta);
        if (isShaking)
        {
            position.x = ((-50 + rand() % 100) / 2.0f * shakingStrength);
            position.y = ((-50 + rand() % 100) / 2.0f * shakingStrength);
        }
    }
    void shake(float strength, float duration)
    {
        isShaking = true;
        shakingStrength = strength;
        timerShake.setWaitTime(duration);
        timerShake.restart();
    }
    void renderTexture(SDL_Texture* texture, const SDL_Rect* rectSrc,
        const SDL_FRect* rectDst, double angle, const SDL_FPoint* center) const
    {
        SDL_FRect rectDstWin = *rectDst;
        rectDstWin.x -= position.x;
        rectDstWin.y -= position.y;

        SDL_RenderCopyExF(renderer, texture, rectSrc, &rectDstWin, angle, center, SDL_RendererFlip::SDL_FLIP_NONE);
    }

private:
    Vector2 position;
    Timer timerShake;
    bool isShaking = false;
    float shakingStrength = 0;
    SDL_Renderer* renderer = nullptr;
};