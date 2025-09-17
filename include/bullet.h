#pragma once
#include"camera.h"
#include"vector2.h"
#include<SDL.h>

extern SDL_Texture* texBullet;

class Bullet
{
public:
    Bullet(double angle)
    {
        this->angle = angle;

        double radians = angle * 3.1415926 / 180.0;
        velocity.x = (float)std::cos(radians) * speed;
        velocity.y = (float)std::sin(radians) * speed;
    }
    ~Bullet() = default;

    void setPosition(const Vector2& position)
    {
        this->position = position;
    }
    const Vector2& getPosition() const
    {
        return position;
    }
    void onUpdate(float delta)
    {
        position += velocity * delta;
        if (position.x <= 0 || position.x >= 1280 || position.y <= 0 || position.y >= 720)
            isValid = false;
    }
    void onRender(const Camera& camera) const
    {
        const SDL_FRect rectBullet = { position.x - 4,position.y - 2,8,4 };
        camera.renderTexture(texBullet, nullptr, &rectBullet, angle, nullptr);
    }

    void onHit()
    {
        isValid = false;
    }

    bool canRemove() const
    {
        return !isValid;
    }
private:
    double angle = 0;
    Vector2 position;
    Vector2 velocity;
    bool isValid = true;
    float speed = 800.0f;
};
extern std::vector<Bullet> bulletsList;