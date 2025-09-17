#define SDL_MAIN_HANDLED
#include  "atlas.h"
#include "camera.h"
#include "bullet.h"
#include"chicken.h"
#include"chicken_fast.h"
#include"chicken_medium.h"
#include"chicken_slow.h"
#include "battery.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include<SDL_image.h>
#include<SDL_mixer.h>

#include<chrono>
#include<thread>
#include<string>
#include<vector>
#include<algorithm>
#include<iostream>


Camera* camera = nullptr;
Battery* Battery::instance = nullptr;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

bool isQuit = false;    //是否退出程序

SDL_Texture* texHeart = nullptr;     //生命值图标纹理
SDL_Texture* texBullet = nullptr;     //子弹纹理
SDL_Texture* texbattery = nullptr;    //炮台基座纹理
SDL_Texture* texCrosshair = nullptr;  //光标准星纹理
SDL_Texture* texBackground = nullptr;  //背景纹理
SDL_Texture* texBarrelIdle = nullptr;  //炮管默认状态纹理

Atlas atlasBarrelFire;  //炮管开火动画图集
Atlas atlasChickenFast; //快速僵尸鸡图集
Atlas atlasChickenMedium; //中速僵尸鸡图集
Atlas atlasChickenSlow; //慢速僵尸鸡图集
Atlas atlasExplosion;  //僵尸鸡死亡爆炸动画图集

Mix_Music* musicBgm = nullptr; //背景音乐
Mix_Music* musicLoss = nullptr; //游戏失败音乐

Mix_Chunk* soundHurt = nullptr; //生命值降低音效
Mix_Chunk* soundFire[3] = { nullptr }; //开火音效
Mix_Chunk* soundExplosion = nullptr; //僵尸鸡死亡爆炸音效

TTF_Font* font = nullptr; //得分文本字体

int hp = 10;          //生命值
int score = 0;     //游戏得分
std::vector<Bullet> bulletsList; //子弹列表
std::vector<Chicken*> chickensList; //僵尸鸡列表
int numPerGen = 2; //每次生成僵尸鸡数量
Timer timerGenerate; //生成僵尸鸡定时器
Timer timerIncreaseNumPerGen;    //增加生成僵尸鸡数量定时器

Vector2 posCrosshair; //准星位置

void loadResources();    //加载游戏资源
void unloadResources();  //卸载游戏资源
void init();              //游戏程序初始化
void deinit();            //游戏程序反初始化
void onUpdate(float delta); //游戏逻辑更新
void onRender(const Camera& camera);          //画面渲染
void mainLoop();          //游戏主循环

int main(int argc, char* argv)
{
    init();
    mainLoop();
    deinit();
    return 0;
}

void init()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    Mix_Init(MIX_INIT_MP3);
    TTF_Init();

    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    Mix_AllocateChannels(32);

    window = SDL_CreateWindow("Zombie Chicken Killer",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 720, SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_ShowCursor(SDL_DISABLE);

    loadResources();

    camera = new Camera(renderer);

    timerGenerate.setOneShot(false);
    timerGenerate.setWaitTime(1.5f);
    timerGenerate.setOnTimeout([&]()
        {
            for (int i = 0; i < numPerGen; i++)
            {
                int val = rand() % 100;
                Chicken* chicken = nullptr;
                if (val < 50)     //50%
                    chicken = new ChickenSlow();
                else if (val < 80) //30%
                    chicken = new ChickenMedium();
                else    //20%
                    chicken = new ChickenFast();

                chickensList.push_back(chicken);
            }
        });
    timerIncreaseNumPerGen.setOneShot(false);
    timerIncreaseNumPerGen.setWaitTime(8.0f);
    timerIncreaseNumPerGen.setOnTimeout([&]()
        {
            numPerGen += 1;
        });

    Mix_PlayMusic(musicBgm, -1);
    Battery::getInstance();
}
void deinit()
{
    delete camera;
    unloadResources();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
}


void loadResources()
{
    texHeart = IMG_LoadTexture(renderer, "res/heart.png");
    texBullet = IMG_LoadTexture(renderer, "res/bullet.png");
    texbattery = IMG_LoadTexture(renderer, "res/battery.png");
    texCrosshair = IMG_LoadTexture(renderer, "res/crosshair.png");
    texBackground = IMG_LoadTexture(renderer, "res/background.png");
    texBarrelIdle = IMG_LoadTexture(renderer, "res/barrel_idle.png");

    atlasBarrelFire.load("res/barrel_fire_%d.png", 3);
    atlasChickenFast.load("res/chicken_fast_%d.png", 4);
    atlasChickenMedium.load("res/chicken_medium_%d.png", 6);
    atlasChickenSlow.load("res/chicken_slow_%d.png", 8);
    atlasExplosion.load("res/explosion_%d.png", 5);

    musicBgm = Mix_LoadMUS("res/bgm.mp3");
    musicLoss = Mix_LoadMUS("res/loss.mp3");

    soundHurt = Mix_LoadWAV("res/hurt.wav");
    soundFire[0] = Mix_LoadWAV("res/fire_1.wav");
    soundFire[1] = Mix_LoadWAV("res/fire_2.wav");
    soundFire[2] = Mix_LoadWAV("res/fire_3.wav");
    soundExplosion = Mix_LoadWAV("res/explosion.wav");

    font = TTF_OpenFont("res/font.ttf", 28);
}


void unloadResources()
{
    SDL_DestroyTexture(texHeart);
    SDL_DestroyTexture(texBullet);
    SDL_DestroyTexture(texbattery);
    SDL_DestroyTexture(texCrosshair);
    SDL_DestroyTexture(texBackground);
    SDL_DestroyTexture(texBarrelIdle);

    Mix_FreeMusic(musicBgm);
    Mix_FreeMusic(musicLoss);

    Mix_FreeChunk(soundHurt);
    Mix_FreeChunk(soundFire[0]);
    Mix_FreeChunk(soundFire[1]);
    Mix_FreeChunk(soundFire[2]);
    Mix_FreeChunk(soundExplosion);
}

void mainLoop()
{
    using namespace std::chrono;
    SDL_Event event;

    const nanoseconds frameDuration(1000000000 / 144);
    steady_clock::time_point lastTick = steady_clock::now();
    while (!isQuit)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                isQuit = true;
                break;
            case SDL_MOUSEMOTION:
                posCrosshair.x = (float)event.motion.x;
                posCrosshair.y = (float)event.motion.y;
                break;
            }
            Battery::getInstance()->input(event);
        }
        steady_clock::time_point frameStart = steady_clock::now();
        duration<float>delta = duration<float>(frameStart - lastTick);

        onUpdate(delta.count());
        onRender(*camera);

        SDL_RenderPresent(renderer);

        lastTick = frameStart;
        nanoseconds sleepDuration = frameDuration - (steady_clock::now() - frameStart);

        if (sleepDuration > nanoseconds(0))
            std::this_thread::sleep_for(sleepDuration);
    }
}

void onUpdate(float delta)
{
    timerGenerate.onUpdate(delta);
    timerIncreaseNumPerGen.onUpdate(delta);

    //更新子弹列表
    for (Bullet& bullet : bulletsList)
        bullet.onUpdate(delta);
    //更新僵尸鸡对象并处理子弹碰撞
    for (Chicken* chicken : chickensList)
    {
        chicken->onUpdate(delta);
        for (Bullet& bullet : bulletsList)
        {
            if (!chicken->checkAlive() || bullet.canRemove()) continue;

            const Vector2& posBullet = bullet.getPosition();
            const Vector2& posChicken = chicken->getPosition();
            static const Vector2 sizeChicken = { 30,40 };
            if (posBullet.x >= posChicken.x - sizeChicken.x / 2
                && posBullet.x <= posChicken.x + sizeChicken.x / 2
                && posBullet.y >= posChicken.y - sizeChicken.y / 2
                && posBullet.y <= posChicken.y + sizeChicken.y / 2)
            {
                score += 1;
                bullet.onHit();
                chicken->onHurt();
            }
        }

        if (!chicken->checkAlive()) continue;

        //漏网之鸡减少剩余生命值
        if (chicken->getPosition().y > 720)
        {
            chicken->makeInvalid();
            Mix_PlayChannel(-1, soundHurt, 0);
            hp -= 1;
        }
        //移除无效的子弹对象
        bulletsList.erase(std::remove_if(bulletsList.begin(), bulletsList.end(),
            [](const Bullet& bullet)
            {
                return bullet.canRemove();
            }),
            bulletsList.end());


        //移除无效的僵尸鸡对象
        chickensList.erase(std::remove_if(chickensList.begin(), chickensList.end(),
            [](const Chicken* chicken)
            {
                bool canRemove = chicken->canRemove();
                if (canRemove)delete chicken;
                return canRemove;
            }),
            chickensList.end());

        //对场景中的僵尸鸡按竖直坐标位置排序
        std::sort(chickensList.begin(), chickensList.end(),
            [](const Chicken* chicken_1, const Chicken* chicken_2)
            {
                return chicken_1->getPosition().y < chicken_2->getPosition().y;
            });
    }

    Battery::getInstance()->update(delta);
    //更新摄像机位置
    camera->onUpdate(delta);

    //检查游戏是否结束
    if (hp <= 0)
    {
        isQuit = true;
        Mix_HaltMusic();
        Mix_PlayMusic(musicLoss, 0);
        std::string msg = "游戏最终得分：" + std::to_string(score);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "游戏结束", msg.c_str(), window);
    }
}
void onRender(const Camera& camera)
{
    //绘制背景图
    {
        int width_bg, height_bg;
        SDL_QueryTexture(texBackground, nullptr, nullptr, &width_bg, &height_bg);
        const SDL_FRect rectDst = {
            (1280 - width_bg) / 2.f,
            (720 - height_bg) / 2.f,
            (float)width_bg,
            (float)height_bg
        };
        camera.renderTexture(texBackground, nullptr, &rectDst, 0, nullptr);

        //绘制僵尸鸡对象
        for (Chicken* chicken : chickensList)
            chicken->onRender(camera);

        //绘制子弹
        for (const Bullet& bullet : bulletsList)
            bullet.onRender(camera);

        Battery::getInstance()->draw(camera);
        {
            //绘制生命值
            {
                int widthHeart, heightHeart;
                SDL_QueryTexture(texHeart, nullptr, nullptr, &widthHeart, &heightHeart);
                for (int i = 0; i < hp; i++)
                {
                    const SDL_Rect rectDst = { 15 + (widthHeart + 10) * i, 15, widthHeart, heightHeart };

                    SDL_RenderCopy(renderer, texHeart, nullptr, &rectDst);
                }
            }

            //绘制游戏得分
            {
                std::string strScore = "Score:" + std::to_string(score);
                SDL_Surface* sufScoreBg = TTF_RenderUTF8_Blended(font, strScore.c_str(), { 55,55,55,255 });
                SDL_Surface* sufScoreFg = TTF_RenderUTF8_Blended(font, strScore.c_str(), { 255,255,255,255 });
                SDL_Texture* texScoreBg = SDL_CreateTextureFromSurface(renderer, sufScoreBg);
                SDL_Texture* texScoreFg = SDL_CreateTextureFromSurface(renderer, sufScoreFg);
                SDL_Rect rectDstScore = { 1280 - sufScoreBg->w - 15,15,sufScoreBg->w,sufScoreBg->h };
                SDL_RenderCopy(renderer, texScoreBg, nullptr, &rectDstScore);
                rectDstScore.x -= 2, rectDstScore.y -= 2;
                SDL_RenderCopy(renderer, texScoreFg, nullptr, &rectDstScore);
                SDL_DestroyTexture(texScoreBg);
                SDL_DestroyTexture(texScoreFg);
                SDL_FreeSurface(sufScoreBg);
                SDL_FreeSurface(sufScoreFg);
            }

            //绘制准星
            {
                int width_crosshair, height_crosshair;
                SDL_QueryTexture(texCrosshair, nullptr, nullptr, &width_crosshair, &height_crosshair);
                const SDL_FRect rectDst = {
                    posCrosshair.x - width_crosshair / 2.f,
                    posCrosshair.y - height_crosshair / 2.f,
                    (float)width_crosshair,
                    (float)height_crosshair
                };
                camera.renderTexture(texCrosshair, nullptr, &rectDst, 0, nullptr);
            }
        }
    }
}