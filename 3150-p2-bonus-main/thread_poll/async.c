#include <stdlib.h>
#include <pthread.h>
#include "async.h"
#include "utlist.h"
#include <unistd.h>
#include <stdio.h>

my_pool_t *pool;

void async_init(int num_threads)
{
    int i;
    pool = (my_pool_t *)malloc(sizeof(my_pool_t));
    pool->queue_size = num_threads;
    pool->head = pool->tail = pool->count = 0;
    pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * num_threads);
    pool->queue = (one_task *)malloc(sizeof(one_task) * num_threads);

    pthread_mutex_init(&(pool->lock), NULL);
    pthread_cond_init(&(pool->call), NULL);

    void * args = NULL;

    for(i = 0; i < num_threads; i++) {
        pthread_create(&(pool->threads[i]), NULL,one_thread, (void*)args);
    }
 
}


void async_run(void (*hanlder)(int), int args)
{
    int next;
    pthread_mutex_lock(&(pool->lock));
    next = (pool->tail + 1) % pool->queue_size;

    pool->queue[pool->tail].function = hanlder;
    pool->queue[pool->tail].argument = args;
    pool->tail = next;
    pool->count += 1;
    pthread_cond_signal(&(pool->call));
    pthread_mutex_unlock(&pool->lock);

}

static void *one_thread(void *t)
{
    one_task task;
    while(1) {
        pthread_mutex_lock(&(pool->lock));
        while(pool->count == 0) {
            pthread_cond_wait(&(pool->call), &(pool->lock));
        }
        if(pool->count == 0) {
            break;
        }
        task.function = pool->queue[pool->head].function;
        task.argument = pool->queue[pool->head].argument;

        pool->head = (pool->head + 1) % pool->queue_size;
        pool->count--;

        pthread_mutex_unlock(&(pool->lock));

        (*(task.function))(task.argument);
    }

    pthread_exit(NULL);
    return(NULL);
}