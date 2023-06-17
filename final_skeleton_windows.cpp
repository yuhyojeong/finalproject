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
#include "keyboard_controller.cpp"

using namespace std;

//main function
int main(int argc, char *argv[])
{   
    ifstream input;
    string input_file = string(argv[1]);
    if(argc!=2){
        cerr<<"Wrong arguments"<<endl;
    }
    input.open(input_file); //an input_file
    if(!(input.is_open())){
        cerr<<"Input missing"<<endl;
        return 0;
    }

    Screen_manager manager;
    int num_event, frame_event, y_event, x_event;
    char type_event;
    input >> num_event;
    manager.num_event = num_event;
    for(int i=0; i<num_event; i++){
        input >> frame_event >> type_event >> y_event >> x_event;
        manager.frame_event[i] = frame_event;
        manager.type_event[i] = type_event;
        manager.y_event[i] = y_event;
        manager.x_event[i] = x_event;
        manager.copy_frame[i] = frame_event;
    }
    input.close();

    
    double operation_time=0; //microseconds
    double frame_length = manager.frame_length;

    HANDLE consolehandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO consolecursor;
    consolecursor.bVisible=0;
    consolecursor.dwSize=1;
    SetConsoleCursorInfo(consolehandle, &consolecursor);
    system("cls");
    system("mode con:cols=62 lines=32");
    manager.print();
    manager.render();

    int ch = 0;
    while(ch !='x' && manager.end == false)
    {
        auto start = std::chrono::system_clock::now();
        if(_kbhit()) {
            ch = getKeyDown();
            manager.print(ch);
            if(ch=='x') break;
            if (ch == 'p'){
                getch();
            }
        }
        else{
            manager.print();
        }
        auto end = std::chrono::system_clock::now();
        auto microsec = std::chrono::duration_cast<std::chrono::microseconds>(end-start);
        int prev_frame = operation_time / frame_length;
        //operation_time += (end-start);
        operation_time += microsec.count();
        manager.curr_frame = operation_time / frame_length;
        
        /*Objects operate every 0.1 seconds.
        Call refresh() every 0.1 seconds(or every 0.1*n seconds if manager.print() takes a long time)
        ex) if operation_time changes to 0.25->0.30, refresh()

        You can modify your manager.print() using multi-threading or not.
        If you use multi-threading, you may print more frequently.
        You can reduce the execution time of manager.print() using multi-threading*/
        if(manager.curr_frame-prev_frame>0){
            manager.render();
            for (int i = 0; i < 5; i++){
                if (manager.dead[i] > 0){
                    manager.score[i] ++;
                    manager.dead[i] = 0;
                }
            }
            if (manager.withenemy > 0){
                manager.my_plane.hp--;
                manager.withenemy = 0;
            }
            if (manager.withbullet >0){
                manager.my_plane.hp -= manager.buldam;
                manager.withbullet = 0;
            }
            //종료 조건
            if (manager.my_plane.hp <= 0){
                manager.end = true;
            }
            int count = 0;
            for (int i = 0; i < num_event; i++){
                if (manager.curr_frame >= manager.frame_event[i]){
                    count++;
                }
            }
            if (manager.enemy.size() == 0 && count == num_event){
                manager.end = true;
            }
        }
    }
    system("cls");
    std::cout<<"Start game~!"<<endl;
    std::cout<<"Your score is ";
    int score;
    for (int i = 0; i < 5; i++){
        score += manager.score[i] * (i+1);
    }
    std::cout<< score << " (n : " << manager.score[0] << " , r : " << manager.score[1] << " , s : " << manager.score[2] << " , d : " << manager.score[3] << " , a : " << manager.score[4] << ")" << endl;
    return 0;
}