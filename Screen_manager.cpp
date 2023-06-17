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