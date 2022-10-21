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


typedef struct threadpool_t threadpool_t;
// typedef struct my_queue {
//   int size;
//   my_item_t *head;

//   /* TODO: More stuff here, maybe? */
// } my_queue_t;

void async_init(int);
void async_run(void (*fx)(int), int args);
static void *threadpool_thread(void *t);
#endif
