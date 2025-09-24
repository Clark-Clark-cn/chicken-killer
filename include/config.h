#pragma once

#include <string>
#include <SDL.h>
#include <unordered_map>
#include "vector2.h"

class ConfigItem;


class ConfigItem{
public:
    std::string value;
    ConfigItem() : value("0") {}
    ConfigItem(const char* value) : value(value) {}
    ConfigItem(int value) : value(std::to_string(value)) {}
    ConfigItem(float value) : value(std::to_string(value)) {}
    ConfigItem(bool value) : value(value ? "true" : "false") {}
    ConfigItem(const Vector2& value) : value("Vector2(" + std::to_string(value.x) + 
    "," + std::to_string(value.y) + ")") {}
    ConfigItem(const SDL_Color& value) : value("Color(" + std::to_string(value.r) + 
    "," + std::to_string(value.g) + "," + std::to_string(value.b) + "," + std::to_string(value.a) + ")") {}
    ~ConfigItem() = default;

    std::string to_string(const std::string& key) const { return (key + "=" + value); }
    static ConfigItem from_string(const std::string& str, std::string& key) {
        size_t pos = str.find('=');
        if (pos == std::string::npos) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Config Error", ("Invalid config line: " + str).c_str(), nullptr);
            return ConfigItem("0");
        }
        key = str.substr(0, pos);
        std::string value = str.substr(pos + 1);
        return ConfigItem(value.c_str());
    }

    std::string asString() const { return value; }
    int asInt() const { return this->operator int(); }
    float asFloat() const { return this->operator float(); }
    bool asBool() const { return this->operator bool(); }
    Vector2 asVector2() const { return this->operator Vector2(); }
    SDL_Color asColor() const { return this->operator SDL_Color(); }

    operator std::string() const { return value; }
    operator int() const { 
        try {
            return std::stoi(value);
        }
        catch (...) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Config Error", "Value is not an integer!", nullptr);
            return 0;
        }
     }
    operator float() const {
        try {
            return std::stof(value);
        }
        catch (...) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Config Error", "Value is not a float!", nullptr);
            return 0.0f;
        }
    }
    operator bool() const {
        if (value == "true" || value == "1") return true;
        if (value == "false" || value == "0") return false;
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Config Error", "Value is not a bool!", nullptr);
        return false;
    }
    operator Vector2() const {
        if (value.substr(0, 8) != "Vector2(" || value.back() != ')') {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Config Error", "Value is not a Vector2!", nullptr);
            return Vector2(0, 0);
        }
        size_t commaPos = value.find(',', 8);
        if (commaPos == std::string::npos) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Config Error", "Value is not a Vector2!", nullptr);
            return Vector2(0, 0);
        }
        try {
            float x = std::stof(value.substr(8, commaPos - 8));
            float y = std::stof(value.substr(commaPos + 1, value.size() - commaPos - 2));
            return Vector2(x, y);
        }
        catch (...) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Config Error", "Value is not a Vector2!", nullptr);
            return Vector2(0, 0);
        }
    }
    operator SDL_Color() const {
        if (value.substr(0, 6) != "Color(" || value.back() != ')') {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Config Error", "Value is not a Color!", nullptr);
            return SDL_Color{ 0,0,0,255 };
        }
        size_t firstComma = value.find(',', 6);
        if (firstComma == std::string::npos) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Config Error", "Value is not a Color!", nullptr);
            return SDL_Color{ 0,0,0,255 };
        }
        size_t secondComma = value.find(',', firstComma + 1);
        if (secondComma == std::string::npos) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Config Error", "Value is not a Color!", nullptr);
            return SDL_Color{ 0,0,0,255 };
        }
        size_t thirdComma = value.find(',', secondComma + 1);
        if (thirdComma == std::string::npos) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Config Error", "Value is not a Color!", nullptr);
            return SDL_Color{ 0,0,0,255 };
        }
        try {
            Uint8 r = static_cast<Uint8>(std::stoi(value.substr(6, firstComma - 6)));
            Uint8 g = static_cast<Uint8>(std::stoi(value.substr(firstComma + 1, secondComma - firstComma - 1)));
            Uint8 b = static_cast<Uint8>(std::stoi(value.substr(secondComma + 1, thirdComma - secondComma - 1)));
            Uint8 a = static_cast<Uint8>(std::stoi(value.substr(thirdComma + 1, value.size() - thirdComma - 2)));
            return SDL_Color{ r,g,b,a };
        }
        catch (...) {
            SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Config Error", "Value is not a Color!", nullptr);
            return SDL_Color{ 0,0,0,255 };
        }
    }
};

class Config
{
    Config(bool isDefault = false) : isDefaultConfig(isDefault){
        if(isDefaultConfig)return;
        loadConfig(defaultConfigPath);
        setDefault();
    }
    ~Config() = default;
public:
    static Config* getInstance()
    {
        if (instance == nullptr)
        {
            defaultConfig = new Config(true);
            instance = new Config(false);
        }
        return instance;
    }
    void setDefault();
    void loadConfig(const std::string& filePath);
    ConfigItem get(const std::string& key) const;
    void set(const std::string& key, const ConfigItem& value);
    void setifno(const std::string& key, const ConfigItem& value);
    void saveConfig(const std::string& filePath)const;

private:
    std::unordered_map<std::string, ConfigItem> configItems;
    const std::string defaultConfigPath = "config.txt";
    static Config* instance, *defaultConfig;
    bool isDefaultConfig = false;
};