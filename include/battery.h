#pragma once
#include "animation.h"
#include "camera.h"
#include "vector2.h"
#include "bullet.h"
#include <SDL.h>
#include <SDL_mixer.h>

extern Atlas atlasBarrelFire;
extern SDL_Texture* texbattery;
extern SDL_Texture* texBarrelIdle;
extern Mix_Chunk* soundFire[3];

extern Camera* camera;

class Battery{
    double angleBarrel = 0; //炮管旋转角度
    const Vector2 posBattery = { 640,600 }; //炮台基座中心位置
    const Vector2 posBarrel = { 592,585 }; //炮管无旋转默认位置
    const SDL_FPoint centerBarrel = { 48,25 }; //炮管旋转中心点坐标
    bool isCoolDown = true; //炮管是否冷却完毕
    bool isFireKeyDown = false; //是否按下开火键
    bool isDoubleFire = false; //是否双倍开火
    Animation animationBarrelFire; //炮管开火动画
    static Battery* instance;
    Timer timerDoubleFire;
    Battery(){
        animationBarrelFire.setLoop(false);
        animationBarrelFire.setInterval(0.04f);
        animationBarrelFire.setCenter(centerBarrel);
        animationBarrelFire.addFrame(&atlasBarrelFire);
        animationBarrelFire.setOnFinished([&]()
            {
                isCoolDown = true;
            });
        animationBarrelFire.setPosition({ 718, 610 });
        timerDoubleFire.setOneShot(true);
        timerDoubleFire.setWaitTime(5.0f);
        timerDoubleFire.setOnTimeout([&]()
            {
                isDoubleFire = false;
            });
    }
    ~Battery() = default;
public:
    static Battery* getInstance(){
        if (instance == nullptr)
            instance = new Battery();
        return instance;
    }
    void setDoubleFire(bool val){ isDoubleFire = val; timerDoubleFire.restart(); }
    bool getDoubleFire() const { return isDoubleFire; }
    void input(const SDL_Event& event)
    {
        switch(event.type){
            case SDL_MOUSEMOTION:
            {
                if(event.button.button == SDL_BUTTON_RIGHT)return;
                Vector2 posCrosshair;
                posCrosshair.x = (float)event.motion.x;
                posCrosshair.y = (float)event.motion.y;
                Vector2 direction = posCrosshair - posBattery;
                float angleBarrel = atan2(direction.y, direction.x) * 180.0 / 3.1415926;
                setAngle(angleBarrel);
            }break;
            case SDL_MOUSEBUTTONDOWN:
                isFireKeyDown = true;
                break;
            case SDL_MOUSEBUTTONUP:
                isFireKeyDown = false;
                break;
        }
    }
    void update(float delta){
        if(!isCoolDown){
            animationBarrelFire.onUpdate(delta);
            if(!camera->isShaking())camera->shake(0.25f, 0.1f);
        }
        if(isCoolDown && isFireKeyDown){
            animationBarrelFire.reset();
            isCoolDown = false;

            if(isDoubleFire)newBullet();
            newBullet();

            Mix_PlayChannel(-1, soundFire[rand() % 3], 0);
        }
        if(isDoubleFire)timerDoubleFire.onUpdate(delta);
    }
    void draw(const Camera& camera){
        //绘制炮台基座
        {
            int width_battery, height_battery;
            SDL_QueryTexture(texbattery, nullptr, nullptr, &width_battery, &height_battery);
            const SDL_FRect rectDst = {
                posBattery.x - width_battery / 2.f,
                posBattery.y - height_battery / 2.f,
                (float)width_battery,
                (float)height_battery
            };
            camera.renderTexture(texbattery, nullptr, &rectDst, 0, nullptr);
            int width_barrel, height_barrel;
            SDL_QueryTexture(texBarrelIdle, nullptr, nullptr, &width_barrel, &height_barrel);
            const SDL_FRect rectBarrel =
            {
              posBarrel.x,
              posBarrel.y,
              (float)width_barrel,
              (float)height_barrel
            };
            if (isCoolDown)
                camera.renderTexture(texBarrelIdle, nullptr, &rectBarrel, angleBarrel, &centerBarrel);
            else
            {
                animationBarrelFire.setRotation(angleBarrel);
                animationBarrelFire.onRender(camera);
            }
        }
        //绘制炮管
        {
            int width_barrel, height_barrel;
            SDL_QueryTexture(texBarrelIdle, nullptr, nullptr, &width_barrel, &height_barrel);
            const SDL_FRect rectBarrel =
            {
              posBarrel.x,
              posBarrel.y,
              (float)width_barrel,
              (float)height_barrel
            };
            if (isCoolDown)
                camera.renderTexture(texBarrelIdle, nullptr, &rectBarrel, angleBarrel, &centerBarrel);
            else
            {
                animationBarrelFire.setRotation(angleBarrel);
                animationBarrelFire.onRender(camera);
            }
            if(isDoubleFire){
                const SDL_FRect rectBarrel =
                {
                  posBarrel.x,
                  posBarrel.y+10,
                  (float)width_barrel,
                  (float)height_barrel
                };
                if (isCoolDown)
                    camera.renderTexture(texBarrelIdle, nullptr, &rectBarrel, angleBarrel, &centerBarrel);
                else
                {
                    animationBarrelFire.setRotation(angleBarrel);
                    animationBarrelFire.onRender(camera);
                }
            }
        }
    }
    void setAngle(double angle){this->angleBarrel = angle;}
    
    Vector2 getPosition() const { return posBattery; }
    void newBullet(){
        static const float lengthBarrel = 105;      //炮管长度
        static const Vector2 posBarrelCenter = { 640,610 };    //炮管瞄点中心位置

        bullets.emplace_back(angleBarrel);      //构造新的子弹对象
        Bullet& bullet = bullets.back();
        double angleBullet = angleBarrel + (rand() % 30 - 15);   //在30度范围内随机偏移
        double radians = angleBullet * 3.14159265 / 180.0;
        bullet.setPosition(posBarrelCenter +
            Vector2{ (float)std::cos(radians) * lengthBarrel,
                (float)std::sin(radians) * lengthBarrel });
    }

};