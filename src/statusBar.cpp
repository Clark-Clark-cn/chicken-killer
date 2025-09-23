#include "statusBar.h"
#include "boom.h"
#include "battery.h"

void StatusBar::update(float delta){
    //检查游戏是否结束
    if (hp <= 0)
    {
        isQuit = true;
        Mix_HaltMusic();
        Mix_PlayMusic(musicLoss, 0);
        std::string msg = "游戏最终得分：" + std::to_string(score);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "游戏结束", msg.c_str(), nullptr);
    }
    if(mp >= 100){
        mp = 100;
    }
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
    const SDL_Rect rectFg = { 15, 15 + heightHeart + 10, mp * mpWidth / 100, mpHeight };
    SDL_SetRenderDrawColor(renderer, 55, 30, 255, 255);
    SDL_RenderFillRect(renderer, &rectFg);

    std::string strScore = "Score:" + std::to_string(score);
    drawText(renderer, strScore, {1278.0f, 30.0f}, font, {55, 55, 55, 255});
    drawText(renderer, strScore, {1280.0f, 32.0f}, font, {255, 255, 255, 255});

    std::string skill1 = "Skill: Bombing (100 MP)";
    drawText(renderer, skill1, {1278.0f, 60.0f}, font, {55, 55, 55, 255});
    if(skillType != SkillType::Bombing)drawText(renderer, skill1, {1280.0f, 62.0f}, font, {255, 255, 255, 255});
    else drawText(renderer, skill1, {1280.0f, 62.0f}, font, {50, 255, 50, 255});

    std::string skill2 = "Skill: doubleFire! (50 MP)";
    drawText(renderer, skill2, {1278.0f, 90.0f}, font, {55, 55, 55, 255});
    if(skillType != SkillType::DoubleFire)drawText(renderer, skill2, {1280.0f, 92.0f}, font, {255, 255, 255, 255});
    else drawText(renderer, skill2, {1280.0f, 92.0f}, font, {50, 255, 50, 255});

    std::string skill3 = "Skill: Heal (30 MP)";
    drawText(renderer, skill3, {1278.0f, 120.0f}, font, {55, 55, 55, 255});
    if(skillType != SkillType::Heal)drawText(renderer, skill3, {1280.0f, 122.0f}, font, {255, 255, 255, 255});
    else drawText(renderer, skill3, {1280.0f, 122.0f}, font, {50, 255, 50, 255});
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
    if (mp >= 100)
    {
        mp -= 100;
        if (mp < 0) mp = 0;
        for(int i=0;i<5;i++)
            Boom::create({ rand() % 1000 + 100.0f, rand() % 400 + 100.0f });
    }
}

void StatusBar::DoubleFire(){
    if (mp >= 50)
    {
        mp -= 50;
        if (mp < 0) mp = 0;
        Battery::getInstance()->setDoubleFire(true);
    }
}

void StatusBar::input(SDL_Event& event){
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
            if(event.key.keysym.sym == SDLK_1){
                skillType = SkillType::Bombing;
            }
            else if(event.key.keysym.sym == SDLK_2){
                skillType = SkillType::DoubleFire;
            }
            else if(event.key.keysym.sym == SDLK_3){
                skillType = SkillType::Heal;
            }
            break;
    }
}