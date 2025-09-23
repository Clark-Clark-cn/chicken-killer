#pragma once
#include <SDL.h>
#include "camera.h"
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
    void increaseMP(int val) { mp += val; if (mp > 100) mp = 100; }
    void setMP(int val) { mp = val; if (mp > 100) mp = 100; if (mp < 0) mp = 0; }
    int getMP() const { return mp; }
    void increaseScore(int val) { score += val; if (score < 0) score = 0; increaseMP(10); }
    int getScore() const { return score; }
    void drawText(SDL_Renderer* renderer, const std::string& text, Vector2 position, TTF_Font* font, SDL_Color color);
    void input(SDL_Event& event);

    void Bomb();
    void DoubleFire();
    void Heal(){ mp -= 30; hp++; if(hp>10)hp=10; }
};