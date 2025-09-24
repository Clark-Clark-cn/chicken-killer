# Zombie Chicken Defense

[中文版](README_cn.md)

This is a simple 2D shooting game developed using C++ and the SDL2 library. Players need to control the character to shoot the constantly appearing zombie chickens and protect their base from being breached.

## Gameplay

- Use the mouse to aim and left-click to shoot zombie chickens.
- Each zombie chicken hit increases the player's score.
- Zombie chickens appear continuously, and players need to hit as many as possible to achieve a high score.
- Health decreases when: Zombie chickens reach the bottom of the screen.
- Game over condition: Health reaches zero.

## Skills
1. Random bombing zone generation
2. Double barrel, double firing rate
3. Restore health

## Installation and Running
1. Clone the repository locally
2. Ensure CMake and Vcpkg are installed
3. Configure CMake
4. Build the project
5. Run the executable
6. Enjoy the game!

## Dependencies
- SDL2
- SDL2_image
- SDL2_mixer
- SDL2_ttf

## AI Usage
This project used GitHub Copilot for code completion.
Used AI to generate parts of CMakeLists.txt and vcpkg.json files.
Used AI to translate README.
