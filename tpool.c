/**
 * @file tpool.c
 * @author Jackson Wildman (jawildm@okstate.edu)
 * @brief Group C
 * @date 04/25/22
 */

/*
 * REFERENCED FROM: MATHIAS BROSSARD
 * Copyright (c) 2016, Mathias Brossard <mathias@brossard.org>.
 * All rights reserved.
 * https://github.com/mbrossard/threadpool
 */

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "threadpool.h"

void *keep_thread_alive(void *in_pool) //keep thread in loop to check for new task
{
    tpool_t *temp_pool = (tpool_t*) in_pool;
    tpool_task_t task;

    while(1) //keep thread in loop to prevent having to create/destroy more
    {
        pthread_mutex_lock(&temp_pool->task_mut);

        while(temp_pool->task_count == 0) //wait for next task
        {
            pthread_cond_wait(&NEW_TASK, &temp_pool->task_mut);
        }

        task.function = temp_pool->task_arr[temp_pool->current_task_pos].function; //assign next function call based on queue
        task.arg = temp_pool->task_arr[temp_pool->current_task_pos].arg;
        temp_pool->current_task_pos++; //update task to next index and decrease task count
        temp_pool->task_count--;
        pthread_mutex_unlock(&temp_pool->task_mut);
        (*(task.function))(task.arg); //call next function
    }
}

tpool_t *create_pool(int thread_count, int task_count) //create threadpool
{
    tpool_t *in_pool; //initialize thread pool object
    in_pool = malloc(sizeof(tpool_t));
    in_pool->thread_arr = malloc(thread_count * sizeof(pthread_t));
    in_pool->task_arr = malloc(task_count * sizeof(tpool_task_t));
    in_pool->task_count = 0;
    in_pool->task_add_pos = 0;
    in_pool->current_task_pos = 0;

    for(int i = 0; i < thread_count; i++) //create all threads and keep them alive
    {   
        pthread_create(&in_pool->thread_arr[i], NULL, keep_thread_alive, (void *) in_pool);
    }

    return in_pool;
}