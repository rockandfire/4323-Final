#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

#include "threadpool.h"

struct threadpool_t *threadpool_create(int thread_count, int queue_size){
    struct threadpool_t *pool;

    pool = (threadpool_t *)malloc(sizeof(threadpool_t));

    pool->queue_size = queue_size;
    pool->head = 0;
    pool->tail = 0;
    pool->count = 0;
    pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * thread_count);
    pool->queue = (threadpool_task_t *)malloc(sizeof(threadpool_task_t) * queue_size);

    for(int i = 0; i < thread_count; i++) {
        if(pthread_create(&(pool->threads[i]), NULL, threadpool_thread, (void*) pool) != 0) {
            threadpool_destroy(pool);
            return NULL;
        }
    }

    return pool;
}

int threadpool_add(threadpool_t *pool1, void (*function)(void *),struct person *persons1){
    struct threadpool_t *pool = (struct threadpool_t*)pool1;

    int next;
    next = pool->tail + 1;
    next = (next == pool->queue_size) ? 0 : next;

    pool->queue[pool->tail].function = function;
    pool->queue[pool->tail].argument = persons1;

    pool->tail = next;
    pool->count += 1;

    pthread_cond_signal(&(pool->notify));

    return 0;
}

int threadpool_destroy(threadpool_t *pool){
    free(pool);
    return 0;
}


void* threadpool_thread(void *threadpool)
{
    threadpool_t *pool = (threadpool_t *)threadpool;
    threadpool_task_t task;

    while(1) {

        while((pool->count == 0) ) {
            pthread_cond_wait(&(pool->notify), &(pool->lock));
        }

        task.function = pool->queue[pool->head].function;
        task.argument = pool->queue[pool->head].argument;

        pool->head += 1;
        pool->head = (pool->head == pool->queue_size) ? 0 : pool->head;
        pool->count -= 1;

        pthread_mutex_unlock(&(pool->lock));

        (*(task.function))(task.argument);
    }
}
