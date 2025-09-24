#include "config.h"
#include <fstream>

Config* Config::instance = nullptr;
Config* Config::defaultConfig = nullptr;

void Config::loadConfig(const std::string& filename)
{
    if (isDefaultConfig) return;

    std::ifstream file(filename);
    if (!file.is_open()) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Config Error", ("Failed to open config file: " + filename).c_str(), nullptr);
        return;
    }

    std::string line;
    std::string key;
    while (std::getline(file, line)) {
        auto item = ConfigItem::from_string(line, key);
        if (key.empty()) continue;
        configItems[key] = item;
    }
}

void Config::saveConfig(const std::string& filename) const
{
    if (isDefaultConfig) return;

    std::ofstream file(filename);
    if (!file.is_open()) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Config Error", ("Failed to open config file for writing: " + filename).c_str(), nullptr);
        return;
    }

    for (const auto& item : configItems) {
        file << item.second.to_string(item.first) << std::endl;
    }
}

ConfigItem Config::get(const std::string& key) const{
    auto it = configItems.find(key);
    if (it != configItems.end()) {
        return it->second;
    }
    if (!isDefaultConfig && defaultConfig) {
        return defaultConfig->get(key);
    }
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Config Error", ("Config key not found: " + key).c_str(), nullptr);
    return ConfigItem("0");
}

void Config::set(const std::string& key, const ConfigItem& value){
    configItems[key] = value;
}

void Config::setifno(const std::string& key, const ConfigItem& value){
    if (configItems.find(key) == configItems.end()) {
        configItems[key] = value;
    }
    defaultConfig->set(key, value);
}

void Config::setDefault(){
    if (isDefaultConfig) return;

    setifno("window.width", 1280);
    setifno("window.height", 720);
    setifno("window.fullscreen", false);
    setifno("window.logical_width", 1280);
    setifno("window.logical_height", 720);
    setifno("window.title", "Zombie Chicken Killer");
    setifno("window.fontsize", 28);
    setifno("window.fps", 144);
    setifno("audio.music.volume", 0.5f);
    setifno("audio.sfx.volume", 0.5f);
    setifno("chicken.spawn.interval", 1.5f);
    setifno("chicken.spawn.increase.interval", 8.0f);
    setifno("chicken.spawn.count", 2);
    setifno("chicken.probability.medium", 0.3f);
    setifno("chicken.probability.slow", 0.5f);
    setifno("chicken.probability.fast(auto calculate)", 0);
    setifno("chicken.size",Vector2(30,40));
    setifno("status.maxHp", 10);
    setifno("status.maxMp", 100);
    setifno("status.mpPerHit", 10);
    setifno("status.text.score", "Score:");
    setifno("status.text.skill1", "Skill: Bombing (100 MP)");
    setifno("status.text.skill2", "Skill: Double Fire (50 MP)");
    setifno("status.text.skill3", "Skill: Heal (30 MP)");
    setifno("status.text.selectedColor", SDL_Color{50,255,50,255});
    setifno("status.text.normalColor", SDL_Color{255,255,255,255});
    setifno("status.skill.bombing.mpCost", 100);
    setifno("status.skill.bombing.count", 5);
    setifno("status.skill.doubleFire.mpCost", 50);
    setifno("status.skill.heal.mpCost", 30);
    setifno("keybind.skill1", SDLK_1);
    setifno("keybind.skill2", SDLK_2);
    setifno("keybind.skill3", SDLK_3);
    setifno("chicken.speed.slow", 30.0f);
    setifno("chicken.speed.medium", 50.0f);
    setifno("chicken.speed.fast", 80.0f);
    setifno("boom.size", Vector2(192, 192));
    setifno("boom.speedRatio", 2.0f);
    setifno("boom.offset", 600);
    setifno("boom.shake.time", 0.1f);
    setifno("boom.shake.strength", 5.0f);
    setifno("bullet.speed", 800.0f);
    setifno("battery.time.doubleFire", 5.0f);
    setifno("battery.time.shake", 0.1f);
    setifno("battery.shake.strength", 0.25f);
    setifno("battery.offset.ratio", 30);

    saveConfig(defaultConfigPath);
}