#pragma once
#include "timer.h"
#include "atlas.h"
#include "camera.h"
#include "vector2.h"

#include <SDL.h>

#include<vector>
#include<functional>

class Animation
{
public:
    Animation()
    {
        timer.setOneShot(false);
        timer.setOnTimeout([&]()
            {
                idxFrame++;
                if (idxFrame >= frameList.size())
                {
                    idxFrame = isLoop ? 0 : frameList.size() - 1;
                    if (!isLoop && onFinished) onFinished();
                }
            }
        );  
    }
    ~Animation() = default;

    void reset()
    {
        timer.restart();
        idxFrame = 0;
    }
    void setPosition(const Vector2& setPosition)
    {
        this->position = setPosition;
    }
    void setRotation(double angle)
    {
        this->angle = angle;
    }
    void setCenter(const SDL_FPoint& center)
    {
        this->center = center;
    }
    void setLoop(bool isLoop)
    {
        this->isLoop = isLoop;
    }
    void setInterval(float interval)
    {
        timer.setWaitTime(interval);
    }
    void setOnFinished(std::function<void()> onFinished)
    {
        this->onFinished = onFinished;
    }
    void addFrame(SDL_Texture* texture, int num_h)
    {
        int width, height;
        SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);

        int widthFrame = width / num_h;
        for (int i = 0;i < num_h;i++)
        {
            SDL_Rect rectSrc;
            rectSrc.x = i * widthFrame, rectSrc.y = 0;
            rectSrc.w = widthFrame, rectSrc.h = height;

            frameList.emplace_back(texture, rectSrc);
        }
    }
    void addFrame(Atlas* atlas)
    {
        for (int i = 0;i < atlas->getSize();i++) 
        {
            SDL_Texture* texture = atlas->getTexture(i);
            int width, height;
            SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);

            SDL_Rect rectSrc;
            rectSrc.x = 0, rectSrc.y = 0;
            rectSrc.w = width, rectSrc.h = height;

            frameList.emplace_back(texture, rectSrc);
        }
    }
    void onUpdate(float delta)
    {
        timer.onUpdate(delta);
    }
    void onRender(const Camera& camera)const
    {
        const Frame& frame = frameList[idxFrame];
        const Vector2& posCamera = camera.getPosition();

        SDL_FRect rectDst;
        rectDst.x = position.x - frame.rectSrc.w / 2;
        rectDst.y = position.y - frame.rectSrc.h / 2;
        rectDst.w = (float)frame.rectSrc.w, rectDst.h = (float)frame.rectSrc.h;
        camera.renderTexture(frame.texture, &frame.rectSrc, &rectDst, angle, &center);
    }

private:
    struct Frame
    {
        SDL_Rect rectSrc;
        SDL_Texture* texture = nullptr;

        Frame() = default;
        Frame(SDL_Texture* texture, const SDL_Rect& rectSrc)
            :texture(texture), rectSrc(rectSrc) {
        }
        ~Frame() = default;
    };
private:
    Vector2 position;
    double angle = 0;
    SDL_FPoint center = { 0 };
    Timer timer;
    bool isLoop = true;
    size_t idxFrame = 0;
    std::vector<Frame> frameList;
    std::function<void()> onFinished;
};