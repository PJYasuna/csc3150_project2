#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <curses.h>
#include <termios.h>
#include <fcntl.h>
#include "frog.h"
#include "log.h"

#define ROW 10
#define COLUMN 50
#define TIME_GAP 100000
#define CAPTURE 0
#define MOVE 1
#define RENDER 2


int flag = 0; // 0 means continued, 1 means lose, 2 means win, 3 means quit
Frog *frog = new Frog();
Log *log1 = new Log(1,9,-1);
Log *log2 = new Log(2,8,1);
Log *log3 = new Log(3,10,-1);
Log *log4 = new Log(4,8,1);
Log *log5 = new Log(5,0,-1);
Log *log6 = new Log(6,24,1);
Log *log7 = new Log(7,15,-1);
Log *log8 = new Log(8,11,1);
Log *log9 = new Log(9,19,-1);
Log *logs[] = {log1,log2,log3,log4,log5,log6,log7,log8,log9};


pthread_mutex_t capture_mutex;
pthread_mutex_t moving_mutex;
pthread_mutex_t render_mutex;

pthread_t threads[3];

// pthread_cond_t flag_signal;

char map[ROW+10][COLUMN] ;

// Determine a keyboard is hit or not. If yes, return 1. If not, return 0.
int kbhit(void){
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);

    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);

    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF)
    {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}


void *capture(void *t) {
    
    while (!flag) {
        pthread_mutex_lock(&capture_mutex);
        if (!frog->is_live(logs[frog->x - 1])) {
            flag = 1;

        }
        if (frog->x == 0) {
            flag = 2;
        }
        if (kbhit()) {
            char key = getchar();
            if (key == 'q' or key == 'Q') {
                flag = 3;
            }
            if (key == 'a' or key == 'A') {
                frog->do_move('a');
            }
            if (key == 'd' or key == 'D') {
                frog->do_move('d');
            }
            if (key == 'w' or key == 'W') {
                frog->do_move('w');
            }
            if (key == 's' or key == 'S') {
                frog->do_move('s');
            }
            
        }
        if (frog->x <= 9 and frog->x >= 1) {
            frog->move_with_log(logs[frog->x - 1]);
        }
        
        pthread_mutex_unlock(&capture_mutex);
        
        usleep(TIME_GAP);
    }

    pthread_exit(NULL);
}



void *render(void *t) {
    
    while (!flag) {
        
        pthread_mutex_lock(&render_mutex);
        

        memset( map , 0, sizeof( map ) ) ;
        int i , j ;
        for( i = 1; i < ROW; ++i ){
            for( j = 0; j < COLUMN - 1; ++j )
                map[i][j] = ' ' ;
        }

        for( j = 0; j < COLUMN - 1; ++j )
            map[ROW][j] = map[0][j] = '|' ;

        for( j = 0; j < COLUMN - 1; ++j )
            map[0][j] = map[0][j] = '|' ;

        int left,right;

        // set logs
        for (i = 1; i <= 9; i++) {
            Log * log = logs[i-1];
            left = log->left;
            right = log->right;
            if (left < right) { // normal
                for (j = left; j <= right; j++) {
                    map[i][j] = '=';
                }
            }
            else { // left > right
                for (j = 0; j <= right; j++) {
                    map[i][j] = '=';
                }
                for (j = left; j < COLUMN - 1; j++) {
                    map[i][j] = '=';
                }
            }
        }
        // set frog
        map[frog->x][frog->y] = '0' ;

        // draw
        printf("\033[2J\033[0;0H"); // clear
        for( i = 0; i <= ROW; ++i)
        puts( map[i] );

        pthread_mutex_unlock(&render_mutex);

        usleep(TIME_GAP);
    }
    
    pthread_exit(NULL);
}


void *logs_move( void *t ){

    int i;
    while (!flag) {
        pthread_mutex_lock(&moving_mutex);
        for (i = 0; i <= 8; i++) {
            Log * log = logs[i];
            log->move();
        }
        pthread_mutex_unlock(&moving_mutex);
        usleep(TIME_GAP);
    }
    pthread_exit(NULL);
}


int main( int argc, char *argv[] ){

    pthread_mutex_init(&capture_mutex, NULL);
    pthread_mutex_init(&moving_mutex, NULL);
    pthread_mutex_init(&render_mutex, NULL);

    // Initialize the river map and frog's starting position
    memset( map , 0, sizeof( map ) ) ;
    int i , j ;
    for( i = 1; i < ROW; ++i ){
        for( j = 0; j < COLUMN - 1; ++j )
            map[i][j] = ' ' ;
    }

    for( j = 0; j < COLUMN - 1; ++j )
        map[ROW][j] = map[0][j] = '|' ;

    for( j = 0; j < COLUMN - 1; ++j )
        map[0][j] = map[0][j] = '|' ;


    map[frog->x][frog->y] = '0' ;


    for( i = 0; i <= ROW; ++i)
        puts( map[i] );

    /*  Create pthreads for wood move and frog control.  */
    int res;
    void * args = (void*) 0;

    res = pthread_create(threads + CAPTURE, NULL, capture, args);
    if (res) {
        printf("ERROR: return code from pthread_create is %d", res);
    }

    res = pthread_create(threads + MOVE, NULL, logs_move, args);
    if (res) {
        printf("ERROR: return code from pthread_create is %d", res);
    }

    res = pthread_create(threads + RENDER, NULL, render, args);
    if (res) {
        printf("ERROR: return code from pthread_create is %d", res);
    }
    
    pthread_join(threads[CAPTURE], NULL);
    pthread_join(threads[MOVE], NULL);
    pthread_join(threads[RENDER], NULL);

    printf("\033[2J\033[0;0H");
    if (flag == 1) {
        printf("You lose the game!!\n");
    }
    if (flag == 2) {
        printf("You win the game!!\n");
    }
    if (flag == 3) {
        printf("You exit the game!!\n");
    }

    pthread_mutex_destroy(&capture_mutex);
    pthread_mutex_destroy(&moving_mutex);
    pthread_mutex_destroy(&render_mutex);
    pthread_exit(NULL);
    return 0;

}
