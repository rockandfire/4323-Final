#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

#include "threadpool.h"


/** makes the pool*/
struct threadpool_struct *make_threadpool(int num_of_threads, int size_of_queue){

    struct threadpool_struct *pool;

    /**mallocs the pool*/
    pool = (threadpool_struct *)malloc(sizeof(threadpool_struct));

    /** init pool*/
    pool->size_of_queue = size_of_queue;
    pool->first = 0;
    pool->last = 0;
    pool->counter = 0;
    pool->threads = (pthread_t *)malloc(sizeof(pthread_t) * num_of_threads);
    pool->queue = (threadpool_jobs *)malloc(sizeof(threadpool_jobs) * size_of_queue);

    /**creates threads in the pool*/
    for(int i = 0; i < num_of_threads; i++) {
        if(pthread_create(&(pool->threads[i]), NULL, threadpool_thread, (void*) pool) != 0) {
            free_threadpool(pool);
            return NULL;
        }
    }

    return pool;
}

/**adds jobs to the pool*/
int threadpool_add_job(threadpool_struct *pool1, void (*function)(void *),struct person *persons1){

    struct threadpool_struct *pool = (struct threadpool_struct*)pool1;

    int counter1;
    counter1 = pool->last + 1;

    /**for keeping track of last*/
    if(counter1 == pool->size_of_queue){
        counter1 = 0;
    }

    /** adds functions and args to threads*/
    pool->queue[pool->last].function = function;
    pool->queue[pool->last].args = persons1;

    pool->last = counter1;
    pool->counter += 1;

    pthread_cond_signal(&(pool->notify));

    return 0;
}

/**frees pool*/
int free_threadpool(threadpool_struct *pool){
    free(pool);
    return 0;
}

/**function for threads and inside this the threads start*/
void* threadpool_thread(void *threadpool){


    threadpool_struct *pool = (threadpool_struct *)threadpool;
    threadpool_jobs jobs;

    while(1) {

        while((pool->counter == 0) ) {
            pthread_cond_wait(&(pool->notify), &(pool->lock));
        }

        /**adds function and args*/
        jobs.function = pool->queue[pool->first].function;
        jobs.args = pool->queue[pool->first].args;

        pool->first += 1;

        if(pool->first == pool->size_of_queue){
            pool->first = 0;
        }

        pool->counter -= 1;

        pthread_mutex_unlock(&(pool->lock));

        (*(jobs.function))(jobs.args);
    }
}
