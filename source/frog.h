#ifndef FROG_H
#define FROG_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <curses.h>
#include <termios.h>
#include <fcntl.h>
#include "log.h"

#define ROW 10
#define COLUMN 50

class Frog {

private:
    void move_up() {
        if (this->x >= 0) this->x--;
    }
    void move_left() {
        if (this->y >= 0) this->y--;
    }
    void move_right() {
        if (this->y <= COLUMN - 2) this->y++;
    }
    void move_down() {
        if (this->x <= ROW-1) this->x++;
    }

public:
    int x;
    int y;

    Frog() {
        this->x = ROW;
        this->y = (COLUMN - 1) / 2;
    }

    void do_move(char c) {
        switch (c) {
            case 'w':
                this->move_up();
                break;
            case 'a':
                this->move_left();
                break;
            case 'd':
                this->move_right();
                break;
            case 's':
                this->move_down();
                break;
            default:
                break;
        }
    }

    int is_live(Log *log) {
        if (this->x == 10 or this->x == 0) return 1;
        if (log->left < log->right) {
            if (this->y >= log->left and this->y <= log->right) return 1;
        }
        else {
            if ((this->y >= log->left and this->y < COLUMN - 2) or (this->y <= log->right and this->y > 0)) return 1;
        }
        
        if (this->y <= 0 or this->y >= COLUMN - 2) return 0;
        return 0;
    }

    void move_with_log(Log *log) {
        if (this->x == log->x) this->y = this->y + log->direction;
    }




};


#endif
