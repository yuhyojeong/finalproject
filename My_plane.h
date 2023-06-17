#ifndef MY_PLANE_H
#define MY_PLANE_H

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
#include "Bullet.h"

using namespace std;

class My_plane{ // hp = 5
    public:
        My_plane(int y_value, int x_value, int frame_value) \
        : y(y_value), x(x_value), create_frame_my_plane(frame_value) {};
        int y, x;
        int shot_frame_my_plane=1;
        int create_frame_my_plane; //initialize when it creates
        int check_frame_my_plane=0;
        vector<Bullet*> bullet;
        vector<Bullet*> buff;
        bool powerup = false;
        int level = 1;
        bool lbuff = false;
        int hp = 5;
};

class Enemy_1n {
    public:
        Enemy_1n(int y, int x, char sym, int order, int hp, int score, int cellspeed = 0, int createfr = 0, int damage = 1) : y(y), x(x), sym(sym), order(order), hp(hp), score(score), cellspeed(cellspeed), createfr(createfr), damage(damage) {}
        int y, x, order, hp, score, cellspeed;
        char sym;
        int createfr = 0;
        int damage;
        bool enembuff = false;
        void damageup() {damage++;}
};

class Enemy_2r : public Enemy_1n{
    public:
        Enemy_2r(int y, int x, char sym, int order, int hp, int score, int cellspeed, int create, int damage) : Enemy_1n(y, x, sym, order, hp, score, cellspeed, create, damage) {}
};

class Enemy_3s : public Enemy_1n{
    public:
        Enemy_3s(int y, int x, char sym, int order, int hp, int score, int cellspeed, int create, int damage) : Enemy_1n(y, x, sym, order, hp, score, cellspeed, create, damage) {}
};

class Enemy_4d : public Enemy_1n{
    public:
        Enemy_4d(int y, int x, char sym, int order, int hp, int score, int cellspeed, int create, int damage) : Enemy_1n(y, x, sym, order, hp, score, cellspeed, create, damage) {}
};

class Enemy_5a : public Enemy_1n{
    public:
        Enemy_5a(int y, int x, char sym, int order, int hp, int score, int buffspeed, int create, int damage) : Enemy_1n(y, x, sym, order, hp, score, buffspeed, create, damage) {}
};
#endif