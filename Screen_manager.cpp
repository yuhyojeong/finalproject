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
#include "Screen_manager.h"
#include <thread>

using namespace std;

//move cursor
void cursorYX(int y, int x)
{
    COORD pos;
    pos.X = x;        
    pos.Y = y;            
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

void Screen_manager::render(){
    for(int j=0; j<height; j++){
        for(int i=0; i<width; i++){
            cursorYX(j, i);
            printf("%c", board[j][i]);
        }
    }
}

//print sharing things.
void Screen_manager::print_share(){
    //wall
    for(int i=0; i<width; i++){board[height-1][i]='w';}
    for(int j=0; j<height; j++){board[j][0]='w'; board[j][width-1]='w';}

    int shot_frame, create_frame, check_frame;

    //Bullet part
    shot_frame = this->my_plane.shot_frame_my_plane; // my bullet
    create_frame = this->my_plane.create_frame_my_plane;
    check_frame = this->my_plane.check_frame_my_plane;
    while ((curr_frame-create_frame)/shot_frame - check_frame > 0){ //bullet create
        Bullet* bullet = new Bullet(this->my_plane.y-1+shot_frame, this->my_plane.x, check_frame, this->my_plane.powerup, my_plane.level);
        this->my_plane.bullet.push_back(bullet);

        for(auto iter=this->my_plane.bullet.begin(); iter<this->my_plane.bullet.end(); ){
            for (auto itr = enemy.begin(); itr < enemy.end(); itr++){ //enemy와 bullet 만나는지
                if ((*iter)->powerup == true){
                    if (((*iter)->y == (*itr)->y) && ((*iter)->x <= (*itr)->x + 1) && ((*iter)->x >= (*itr)->x - 1)){
                        (*itr)->hp -= (*iter)->level;
                    }
                } else{
                    if ((*iter)->y == (*itr)->y && (*iter)->x == (*itr)->x){
                        (*itr)->hp -= (*iter)->level;
                    }
                }
            }
            if((*iter)->y<=0){
                extrabullet((*iter)->y, (*iter)->x, (*iter)->powerup);
                board[(*iter)->y][(*iter)->x]=' ';
                this->my_plane.bullet.erase(iter);
            }
            else{
                if(iter!=(this->my_plane.bullet.end()-1) && curr_frame!=1){
                    extrabullet((*iter)->y, (*iter)->x, (*iter)->powerup);
                    board[(*iter)->y][(*iter)->x]=' ';
                }
                (*iter)->y -= shot_frame;
                if ((*iter)->powerup == true){
                    if ((*iter)->x >=2){
                        board[(*iter)->y][(*iter)->x - 1]= (*iter)->symbol[(*iter)->level - 1];
                    }
                    if ((*iter)->x <= 57){
                        board[(*iter)->y][(*iter)->x + 1]= (*iter)->symbol[(*iter)->level - 1];
                    }
                }
                board[(*iter)->y][(*iter)->x]= (*iter)->symbol[(*iter)->level - 1];
                iter++;
            }
        }
        this->my_plane.check_frame_my_plane+=1;
        check_frame++;
    }

    for (auto itr = enembul.begin(); itr < enembul.end(); itr++){ //enemy bullet
        if ((curr_frame - (*itr)->create_frame_bullet) > 0){
            if ((*itr)->y == my_plane.y && (*itr)->x == my_plane.x){ //plane과 enemybullet 만나는지
                my_plane.hpdown((*itr)->damage);
            }
            if ((*itr)->sym == 's' || (*itr)->sym == 'S'){
                board[(*itr)->y][(*itr)->x] = ' ';
                if (board[(*itr)->y + 1][(*itr)->x] != 'w'){
                    board[(*itr)->y + 1][(*itr)->x] = '*';
                    Enemy_bullet* sbul = new Enemy_bullet((*itr)->y + 1, (*itr)->x, (*itr)->sym, (*itr)->damage, curr_frame);
                    enembul.insert(itr, sbul);
                    enembul.erase(itr + 1);
                } else{
                    enembul.erase(itr);
                    itr--;
                }
            } else if ((*itr)->sym == 'd' || (*itr)->sym == 'D'){
                board[(*itr)->y][(*itr)->x] = ' ';
                if ((*itr)->x <= 29){ //왼쪽이 더 가까움
                    if (board[(*itr)->y + 1][(*itr)->x - 1] != 'w'){
                        board[(*itr)->y + 1][(*itr)->x - 1] = '*';
                        Enemy_bullet* dbul = new Enemy_bullet((*itr)->y + 1, (*itr)->x - 1, (*itr)->sym, (*itr)->damage, curr_frame);
                        enembul.insert(itr, dbul);
                        enembul.erase(itr + 1);
                    } else{
                        enembul.erase(itr);
                        itr--;
                    }
                } else{
                    if (board[(*itr)->y + 1][(*itr)->x + 1] != 'w'){
                        board[(*itr)->y + 1][(*itr)->x + 1] = '*';
                        Enemy_bullet* dbul = new Enemy_bullet((*itr)->y + 1, (*itr)->x + 1, (*itr)->sym, (*itr)->damage, curr_frame);
                        enembul.insert(itr, dbul);
                        enembul.erase(itr + 1);
                    } else{
                        enembul.erase(itr);
                        itr--;
                    }
                }
                
            }
        }
    }
    //Bullet part ends

    //object part
    for (int i = 0; i < num_event; i++){
        if (curr_frame == frame_event[i]){
            board[y_event[i]][x_event[i]] = type_event[i];
            switch (type_event[i]){
                case 'P':{
                    Powerup_bullet* pb = new Powerup_bullet(y_event[i], x_event[i], 'P', i);
                    this->my_plane.buff.push_back(pb);
                    break;
                }
                case 'L':{
                    Levelup_bullet* lb = new Levelup_bullet(y_event[i], x_event[i], 'L', i);
                    this->my_plane.buff.push_back(lb);
                    break;
                }
                case 'n':{
                    Enemy_1n* enn = new Enemy_1n(y_event[i], x_event[i], 'n', i, 10, 1);
                    enemy.push_back(enn);
                    break;
                }
                case 'r':{
                    Enemy_2r* enn = new Enemy_2r(y_event[i], x_event[i], 'r', i, 5, 2, 3, frame_event[i], 1);
                    enemy.push_back(enn);
                    break;
                }
                case 's':{
                    Enemy_3s* enn = new Enemy_3s(y_event[i], x_event[i], 's', i, 4, 3, 9, frame_event[i], 1);
                    enemy.push_back(enn);
                    break;
                }
                case 'd':{
                    Enemy_4d* enn = new Enemy_4d(y_event[i], x_event[i], 'd', i, 5, 4, 3, frame_event[i], 1);
                    enemy.push_back(enn);
                    break;
                }
                case 'a':{
                    Enemy_5a* enn = new Enemy_5a(y_event[i], x_event[i], 'a', i, 8, 5, 6, frame_event[i], 1);
                    enemy.push_back(enn);
                    break;
                }
                default:
                    break;
            }
        }
    }

    for (auto elem : enemy){
        board[elem->y][elem->x] = elem->sym;
    }
    for (auto elem : my_plane.buff){
        board[elem->y][elem->x] = elem->sym;
    }

    for (auto itr = enemy.begin(); itr < enemy.end(); itr++){ //enemy buff 주기
        if ((*itr)->sym == 'a' || (*itr)->sym == 'A'){
            for (auto enem = enemy.begin(); enem < enemy.end(); enem++){
                if (enem-enemy.begin()!= itr-enemy.begin() && (*enem)->y <= (*itr)->y + 3 && (*enem)->y >= (*itr)->y - 3 && (*enem)->x <= (*itr)->x + 3 && (*enem)->x >= (*itr)->x - 3){
                    (*enem)->damage ++;
                    switch ((*enem)->sym){
                        case 'n':
                            (*enem)->sym = 'N';
                            break;
                        case 'r':
                            (*enem)->sym = 'R';
                            break;
                        case 's':
                            (*enem)->sym = 'S';
                            break;
                        case 'd':
                            (*enem)->sym = 'D';
                            break;
                    }
                }
            }
        }
    }

    for (auto itr = enemy.begin(); itr < enemy.end(); itr++){ //enemy 이동
        if ((*itr)->sym != 'n' && (*itr)->sym != 'a' && (*itr)->sym != 'N' && (*itr)->sym != 'A'){
            if ((this->curr_frame - (*itr)->createfr)/((*itr)->cellspeed)> 0){
                board[(*itr)->y][(*itr)->x] = ' ';
                if (board[(*itr)->y + 1][(*itr)->x] != 'w'){
                    board[(*itr)->y + 1][(*itr)->x] = (*itr)->sym;
                    Enemy_1n *enn = new Enemy_1n((*itr)->y + 1, (*itr)->x, (*itr)->sym, (*itr)->order, (*itr)->hp, (*itr)->score, (*itr)->cellspeed, curr_frame);
                    if (((*itr)->sym == 's' || (*itr)->sym == 'S') && board[enn->y + 1][enn->x] != 'w'){
                        Enemy_bullet* sbul = new Enemy_bullet(enn->y + 1, enn->x, enn->sym, enn->damage, curr_frame);
                        enembul.push_back(sbul);
                        board[sbul->y][sbul->x] = '*';
                    } else if (((*itr)->sym == 'd' || (*itr)->sym == 'D') && board[enn->y + 1][enn->x + 1] != 'w' && board[enn->y + 1][enn->x - 1] != 'w'){
                        if ((*itr)->x <= 29){ //왼쪽 벽이 더 가까움
                            Enemy_bullet* dbul = new Enemy_bullet(enn->y + 1, enn->x - 1, enn->sym, enn->damage, curr_frame);
                            enembul.push_back(dbul);
                            board[dbul->y][dbul->x] = '*';
                        } else{
                            Enemy_bullet* dbul = new Enemy_bullet(enn->y + 1, enn->x + 1, enn->sym, enn->damage, curr_frame);
                            enembul.push_back(dbul);
                            board[dbul->y][dbul->x] = '*';
                        }
                    }
                    enemy.insert(itr, enn);
                    enemy.erase(itr + 1);
                } else{
                    enemy.erase(itr);
                    itr--;
                }
            }
        }
    }

    for (auto itr = enemy.begin(); itr < enemy.end(); itr++){
        if ((*itr)->hp <= 0){ //적 사망
            board[(*itr)->y][(*itr)->x] = ' ';
            dead[(*itr)->score - 1] ++;
            enemy.erase(itr);
            itr--;
        }
    }

    //object part ends

    //plane part
    board[my_plane.y][my_plane.x] = 'M';
    
    for (auto itr = my_plane.buff.begin(); itr < my_plane.buff.end(); itr++){ //buff 먹는지 확인
        if (my_plane.y == (*itr)->y && my_plane.x == (*itr)->x){
            if ((*itr)->sym == 'P'){
                my_plane.powerup = true;
                my_plane.buff.erase(itr);
                itr--;
            } else if ((*itr)->sym == 'L'){
                my_plane.lbuff = true;
                my_plane.buff.erase(itr);
                itr--;
            }
        }
    }
    if (my_plane.lbuff == true){
        my_plane.level ++;
        my_plane.lbuff = false;
    }

    for (auto itr = enemy.begin(); itr < enemy.end(); itr++){ //적이랑 부딪히는지 확인
        if (my_plane.y == (*itr)->y && my_plane.x == (*itr)->x){
            my_plane.hpdown(1);
        }
    }
}

//print when key didn't pressed
void Screen_manager::print(){
    board[this->my_plane.y][this->my_plane.x]='M';
    print_share();
}

//print when key pressed
void Screen_manager::print(int ch){ //ascii
    if(ch=='d'){ //right
        if(this->my_plane.y >=0 && this->my_plane.y <(height-1) && this->my_plane.x >0 && this->my_plane.x < (width-2)){
            board[this->my_plane.y][this->my_plane.x]=' ';
            this->my_plane.x +=1;
            board[this->my_plane.y][this->my_plane.x]='M';
        }
    }
    else if(ch=='a'){ //left
        if(this->my_plane.y >=0 && this->my_plane.y <(height-1) && this->my_plane.x >1 && this->my_plane.x < (width-1)){
            board[this->my_plane.y][this->my_plane.x]=' ';
            this->my_plane.x -=1;
            board[this->my_plane.y][this->my_plane.x]='M';
        }
    }
    else if(ch=='w'){ //up
        if(this->my_plane.y >0 && this->my_plane.y <(height-1) && this->my_plane.x >0 && this->my_plane.x < (width-1)){
            board[this->my_plane.y][this->my_plane.x]=' ';
            this->my_plane.y -=1;
            board[this->my_plane.y][this->my_plane.x]='M';
        }
    }
    else if(ch=='s'){ //down
        if(this->my_plane.y >=0 && this->my_plane.y <(height-2) && this->my_plane.x >0 && this->my_plane.x < (width-1)){
            board[this->my_plane.y][this->my_plane.x]=' ';
            this->my_plane.y +=1;
            board[this->my_plane.y][this->my_plane.x]='M';
        }    
    }

    print_share();
}

void Screen_manager::extrabullet(int y, int x, bool p){
    if (p == true){
        if (x >=2){
            board[y][x - 1]=' ';
        }
        if (x <= 57){
            board[y][x + 1]=' ';
        }
    }
}