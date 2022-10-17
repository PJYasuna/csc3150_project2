#ifndef LOG_H
#define LOG_H


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <curses.h>
#include <termios.h>
#include <fcntl.h>

#define ROW 10
#define COLUMN 50 

class Log {

public:
    int x;
    int left;
    int right;

    int length;
    int direction; // 1 is right, -1 is left

    Log(int a, int b, int c) {
        this->x = a;
        this->left = b;
        this->length = 15;
        this->direction = c;
        this->right = (this->left + this->length) % (COLUMN - 2);
    }

    void update_right() {
        this->right = (this->left + this->length) % (COLUMN - 2);
    }

    void move() {
        if (this->left == 0 and this->direction == -1) { // move 
            this->left = COLUMN -2;
        }
        else if (this->left == COLUMN - 2 and this->direction == 1) {
            this->left = 0;
        }
        else {
            this->left = this->left + this->direction;
        }
        update_right();
    }




};

#endif