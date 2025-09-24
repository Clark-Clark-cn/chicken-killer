#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <string>
#include <cstdio>

class Atlas

{
public:
    Atlas() = default;
    ~Atlas()
    {
        for (SDL_Texture* texture : tex_list)
            SDL_DestroyTexture(texture);
    }
    void load(SDL_Renderer* renderer, const char* path_template, int num)
    {
        for (int i = 0; i < num; i++)
        {
            char path_file[256];
            std::snprintf(path_file, sizeof(path_file), path_template, i + 1);
            SDL_Texture* texture = IMG_LoadTexture(renderer, path_file);
            tex_list.push_back(texture);
        }
    }
    void load(const std::string& path_template, int num)
    {
        for (int i = 0; i < num; i++)
        {
            char path_file[256];
            std::snprintf(path_file, sizeof(path_file), path_template.c_str(), i + 1);
            SDL_Texture* texture = IMG_LoadTexture(SDL_GetRenderer(SDL_GetWindowFromID(1)), path_file);
            tex_list.push_back(texture);
        }
    }
    void clear()
    {
        tex_list.clear();
    }
    int getSize() const
    {
        return (int)tex_list.size();
    }
    SDL_Texture* getTexture(int idx)
    {
        if (idx < 0 || idx >= tex_list.size())
            return nullptr;

        return tex_list[idx];
    }
    void addTexture(SDL_Texture* texture)
    {
        tex_list.push_back(texture);
    }
private:
    std::vector<SDL_Texture*> tex_list;
};
