#pragma once
#include "vector2.h"
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "camera.h"
#include "atlas.h"
#include "animation.h"
#include "chicken.h"
#include "statusBar.h"
#include "config.h"

class Boom;
extern Camera* camera;
extern std::vector<Boom*> booms;
extern Atlas atlasArray;
extern Atlas atlasBoomExplode;

extern Mix_Chunk* soundBoom;
extern bool isDebug;
extern SDL_Renderer* renderer;

class Boom
{
    Vector2 position;
    Vector2 pos_aim;
    Vector2 sizeArray;
    Vector2 sizeExplode;
    Vector2 velocity = { 0, 300 };
    Animation animationArray;
    Animation animationExplode;
    Timer timerExplode;
    bool isExplode = false;
    bool isReached = false;
    bool canRemove = false;
    void killChickens(){
        for (auto& chicken : chickens)
        {
            if (chicken->getPosition().distance(position) < sizeExplode.length() / 2)
            {
                chicken->onHurt();
                StatusBar::getInstance()->increaseScore(1);
            }
        }
    }
public:
    Boom(){
        sizeArray = {38,38};
        sizeExplode = Config::getInstance()->get("boom.size");
        timerExplode.setWaitTime(1.0f);
        timerExplode.setOneShot(true);
        timerExplode.setOnTimeout([&]() {
            Mix_PlayChannel(-1, soundBoom, 0);
            isExplode=true;
            killChickens();
            static const float shakeStrength = Config::getInstance()->get("boom.shake.strength");
            static const float shakeTime = Config::getInstance()->get("boom.shake.time");
            camera->shake(shakeStrength, shakeTime);
        });
        animationArray.setLoop(true);
        animationArray.setInterval(0.3f);
        animationArray.addFrame(&atlasArray);
        animationExplode.setLoop(false);
        animationExplode.setInterval(0.1f);
        animationExplode.addFrame(&atlasBoomExplode);
        animationExplode.setOnFinished([&]() {
            canRemove = true;
        });
    }
    ~Boom() = default;
    static Boom* create(const Vector2& pos_aim)
    {
        booms.emplace_back(new Boom());
        Boom* boom=booms.back();
        boom->position.x = pos_aim.x;
        boom->position.y = pos_aim.y + Config::getInstance()->get("boom.offset").asInt();
        boom->pos_aim = pos_aim;
        return boom;
    }
    const Vector2& getPosition() const { return position; }
    void setPosition(const Vector2& pos) { position = pos; }
    void onUpdate(float delta)
    {
        static float speedRatio = Config::getInstance()->get("boom.speedRatio");
        if(!isReached){
            position += velocity * delta;
            velocity.y = position.y > pos_aim.y ? 
            -(position.y - pos_aim.y)*speedRatio : (pos_aim.y-position.y)*speedRatio;
            velocity.x = (pos_aim.x - position.x);
            if(velocity.y>-0.03f)position=pos_aim;
        }
        if(position==pos_aim){
            if(!isReached){
                position = pos_aim;
                velocity = { 0,0 };
                timerExplode.restart();
                isReached = true;
            }
            timerExplode.onUpdate(delta);
        }
        animationArray.onUpdate(delta);
        animationArray.setPosition(position);
        animationExplode.setPosition(position);
        if(isExplode) animationExplode.onUpdate(delta);
    }
    void draw() const
    {
        if (isExplode)
            animationExplode.onRender(*camera);
        else
            animationArray.onRender(*camera);

        if(isDebug){
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_RenderDrawPointF(renderer, position.x, position.y);
            SDL_Rect rect = { (int)(position.x - sizeExplode.x / 2),(int)(position.y - sizeExplode.y / 2),
                (int)sizeExplode.x,(int)sizeExplode.y };
            SDL_RenderDrawRect(renderer, &rect);
        }
    }

    static void updateAll(float delta)
    {
        booms.erase(std::remove_if(booms.begin(), booms.end(), [](const Boom* boom) {
            if(boom->canRemove) {
                delete boom;
                return true;
            }
            return boom->canRemove;
        }), booms.end());
        for (auto& boom : booms)
        {
            boom->onUpdate(delta);
        }
    }
    static void drawAll()
    {
        for (const auto& boom : booms)
        {
            boom->draw();
        }
    }
};