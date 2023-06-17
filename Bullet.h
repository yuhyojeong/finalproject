#ifndef BULLET_H
#define BULLET_H

#include <stdio.h>
#include <locale.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <Windows.h>
#include <conio.h>
#include <chrono>
#include <vector>

class Bullet{
    public:
        Bullet(int y_value, int x_value, int frame_value, bool powerup, int level) \
        : y(y_value), x(x_value), create_frame_bullet(frame_value), powerup(powerup), level(level) {};
        Bullet(int y, int x, char sym, int order) : y(y), x(x), sym(sym), order(order) {}
        Bullet(int y, int x, char sym, int damage, int frame) : y(y), x(x), sym(sym), damage(damage), create_frame_bullet(frame) {}
        int damage=1;
        int y, x;
        int create_frame_bullet; //initialize when it creates
        int check_frame_bullet;
        int cellspeed = 1;
        char symbol[3] = {'\'', '^', '!'};
        char sym;
        int order; //event 순서
        bool powerup = false;
        int level = 1;
        bool damagebuff = false;
};

class Powerup_bullet : public Bullet{
    public:
        Powerup_bullet(int y, int x, char sym, int order) : Bullet(y, x, sym, order) {}
};

class Levelup_bullet : public Bullet{
    public:
        Levelup_bullet(int y, int x, char sym, int order) : Bullet(y, x, sym, order) {}
};

class Enemy_bullet : public Bullet{
    public:
        Enemy_bullet(int y, int x, char enemsym, int damage, int frame) : Bullet(y, x, enemsym, damage, frame) {}
};
#endif