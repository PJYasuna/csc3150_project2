#ifndef __ASYNC__
#define __ASYNC__

#include <pthread.h>


// typedef struct my_item {
//   /* TODO: More stuff here, maybe? */
//   int call;
//   void (*hanlder)(int);
//   int args;
//   struct my_item *next;
//   struct my_item *prev;
// } my_item_t;



// typedef struct my_queue {
//   int size;
//   my_item_t *head;

//   /* TODO: More stuff here, maybe? */
// } my_pool;
typedef struct {
    void (*function)(int);
    int argument;
} one_task;

typedef struct my_pool {
  pthread_mutex_t lock;
  pthread_cond_t call;
  pthread_t *threads;
  one_task *queue;
  int queue_size;
  int head;
  int tail;
  int count;
} my_pool_t;

void async_init(int);
void async_run(void (*fx)(int), int args);
static void *one_thread(void *t);
#endif