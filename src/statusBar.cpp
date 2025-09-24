#include "statusBar.h"
#include "boom.h"
#include "battery.h"
#include "config.h"

void StatusBar::update(float delta){
    //检查游戏是否结束
    if (hp <= 0)
    {
        isQuit = true;
        Mix_HaltMusic();
        Mix_PlayMusic(musicLoss, 0);
        std::string msg = "Final Score: " + std::to_string(score);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Game Over", msg.c_str(), nullptr);
    }
    if(mp >= max_mp){
        mp = max_mp;
    }
}

void StatusBar::increaseScore(int val) {
    score += val; 
    if (score < 0) score = 0; 
    increaseMP(Config::getInstance()->get("status.mpPerHit")); 
}

void StatusBar::draw(SDL_Renderer* renderer){
    int widthHeart, heightHeart;
    SDL_QueryTexture(texHeart, nullptr, nullptr, &widthHeart, &heightHeart);
    for (int i = 0; i < hp; i++)
    {
        const SDL_Rect rectDst = { 15 + (widthHeart + 10) * i, 15, widthHeart, heightHeart };

        SDL_RenderCopy(renderer, texHeart, nullptr, &rectDst);
    }
    int mpWidth = 200;
    int mpHeight = 10;
    const SDL_Rect rectBg = { 15, 15 + heightHeart + 10, mpWidth, mpHeight };
    SDL_SetRenderDrawColor(renderer, 55, 55, 55, 255);
    SDL_RenderFillRect(renderer, &rectBg);
    const SDL_Rect rectFg = { 15, 15 + heightHeart + 10, mp * mpWidth / max_mp, mpHeight };
    SDL_SetRenderDrawColor(renderer, 55, 30, 255, 255);
    SDL_RenderFillRect(renderer, &rectFg);

    static const SDL_Color selectedColor = Config::getInstance()->get("status.text.selectedColor").asColor();
    static const SDL_Color normalColor = Config::getInstance()->get("status.text.normalColor").asColor();

    std::string strScore = Config::getInstance()->get("status.text.score").asString() + std::to_string(score);
    drawText(renderer, strScore, {1278.0f, 30.0f}, font, {55, 55, 55, 255});
    drawText(renderer, strScore, {1280.0f, 32.0f}, font, normalColor);

    std::string skill1 = Config::getInstance()->get("status.text.skill1").asString();
    drawText(renderer, skill1, {1278.0f, 60.0f}, font, {55, 55, 55, 255});
    if(skillType != SkillType::Bombing)drawText(renderer, skill1, {1280.0f, 62.0f}, font, normalColor);
    else drawText(renderer, skill1, {1280.0f, 62.0f}, font, selectedColor);

    std::string skill2 = Config::getInstance()->get("status.text.skill2").asString();
    drawText(renderer, skill2, {1278.0f, 90.0f}, font, {55, 55, 55, 255});
    if(skillType != SkillType::DoubleFire)drawText(renderer, skill2, {1280.0f, 92.0f}, font, normalColor);
    else drawText(renderer, skill2, {1280.0f, 92.0f}, font, selectedColor);

    std::string skill3 = Config::getInstance()->get("status.text.skill3").asString();
    drawText(renderer, skill3, {1278.0f, 120.0f}, font, {55, 55, 55, 255});
    if(skillType != SkillType::Heal)drawText(renderer, skill3, {1280.0f, 122.0f}, font, normalColor);
    else drawText(renderer, skill3, {1280.0f, 122.0f}, font, selectedColor);
}

void StatusBar::drawText(SDL_Renderer* renderer, const std::string& text, Vector2 position, TTF_Font* font, SDL_Color color){
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect rectDst = { (int)position.x-surface->w, (int)position.y-surface->h, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, nullptr, &rectDst);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void StatusBar::Bomb(){
    static const int mpCost = Config::getInstance()->get("status.skill.bombing.mpCost");
    static const int count = Config::getInstance()->get("status.skill.bombing.count");
    if (mp >= mpCost)
    {
        mp -= mpCost;
        for(int i=0;i<count;i++)
            Boom::create({ rand() % 1000 + 100.0f, rand() % 400 + 100.0f });
    }
}

void StatusBar::DoubleFire(){
    static const int mpCost = Config::getInstance()->get("status.skill.doubleFire.mpCost");
    if (mp >= mpCost)
    {
        mp -= mpCost;
        Battery::getInstance()->setDoubleFire(true);
    }
}

void StatusBar::Heal(){
    static const int mpCost = Config::getInstance()->get("status.skill.heal.mpCost");
    if (mp >= mpCost && hp < max_hp)
    {
        mp -= mpCost;
        hp++;
    }
}

void StatusBar::input(SDL_Event& event){
    static const int key1 = Config::getInstance()->get("keybind.skill1");
    static const int key2 = Config::getInstance()->get("keybind.skill2");
    static const int key3 = Config::getInstance()->get("keybind.skill3");
    switch(event.type){
        case SDL_MOUSEWHEEL:
            if(event.wheel.y > 0){
                if(skillType == SkillType::Bombing)skillType = SkillType::Heal;
                else skillType = static_cast<SkillType>(static_cast<int>(skillType)-1);
            }
            else if(event.wheel.y < 0){
                if(skillType == SkillType::Heal)skillType = SkillType::Bombing;
                else skillType = static_cast<SkillType>(static_cast<int>(skillType)+1);
            }
            break;
        case SDL_MOUSEBUTTONDOWN:{
            if(event.button.button == SDL_BUTTON_RIGHT){
                switch (skillType)
                {
                case SkillType::Bombing:
                    StatusBar::getInstance()->Bomb();
                    break;
                case SkillType::DoubleFire:
                    StatusBar::getInstance()->DoubleFire();
                    break;
                case SkillType::Heal:
                    StatusBar::getInstance()->Heal();
                    break;
                default:
                    break;
                }
            }
        }break;
        case SDL_KEYDOWN:
            if(event.key.keysym.sym == key1){
                skillType = SkillType::Bombing;
            }
            else if(event.key.keysym.sym == key2){
                skillType = SkillType::DoubleFire;
            }
            else if(event.key.keysym.sym == key3){
                skillType = SkillType::Heal;
            }
            break;
    }
}