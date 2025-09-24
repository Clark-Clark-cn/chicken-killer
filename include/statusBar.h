#pragma once
#include <SDL.h>
#include "camera.h"
#include "config.h"
#include <string>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

extern SDL_Texture* texHeart;
extern bool isQuit;
extern Mix_Music* musicLoss;
extern TTF_Font* font;


enum class SkillType{
    None,
    Bombing,
    DoubleFire,
    Heal
};


class StatusBar
{
    int hp = 10;
    int mp = 0;
    const int max_hp = Config::getInstance()->get("status.maxHp");
    const int max_mp = Config::getInstance()->get("status.maxMp");
    int score = 0;
    SkillType skillType = SkillType::Bombing;
    static StatusBar* instance;
    StatusBar() = default;
    ~StatusBar() = default;
public:
    static StatusBar* getInstance()
    {
        if (!instance)
            instance = new StatusBar();
        return instance;
    }

    void draw(SDL_Renderer* renderer);
    void update(float delta);
    void decreaseHP(int val) { hp -= val; if (hp < 0) hp = 0; }
    void increaseMP(int val) { mp += val; if (mp > max_mp) mp = max_mp; }
    void setMP(int val) { mp = val; if (mp > max_mp) mp = max_mp; if (mp < 0) mp = 0; }
    int getMP() const { return mp; }
    void increaseScore(int val);
    int getScore() const { return score; }
    void drawText(SDL_Renderer* renderer, const std::string& text, Vector2 position, TTF_Font* font, SDL_Color color);
    void input(SDL_Event& event);

    void Bomb();
    void DoubleFire();
    void Heal();
};