/**
 * @file tpool.c
 * @author Jackson Wildman (jawildm@okstate.edu)
 * @brief Group C
 * @date 04/12/22
 */

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "threadpool.h"

/*
 * PARTIALLY BASED ON AN EXAMPLE FROM MATHIAS BROSSARD
 * Copyright (c) 2016, Mathias Brossard <mathias@brossard.org>.
 * All rights reserved.
 * https://github.com/mbrossard/threadpool
 */

//BEGIN EXAMPLE-BASED METHODS***********************************************************************************

void *keep_thread_alive(void *in_pool) //keep thread in loop to check for new task
{
    tpool_t *temp_pool = (tpool_t*) in_pool;
    tpool_task_t task;

    while(1) //keep thread in loop to prevent having to create/destroy more
    {
        pthread_mutex_lock(&temp_pool->task_mut);

        while(temp_pool->task_count == 0) //wait for next task
        {
            pthread_cond_wait(&temp_pool->new_task, &temp_pool->task_mut);
        }

        task.function = temp_pool->task_arr[temp_pool->current_task_pos].function; //assign next function call based on queue
        task.arg = temp_pool->task_arr[temp_pool->current_task_pos].arg;
        temp_pool->current_task_pos++; //update task to next index and decrease task count
        temp_pool->task_count--;
        pthread_mutex_unlock(&temp_pool->task_mut);
        (*(task.function))(task.arg); //call next function
    }
}

void pool_new_task(tpool_t *in_pool, void (*function) (void*), struct person *in_person) //add new task to thread pool
{
    tpool_t *temp_pool = (tpool_t*) in_pool;
    temp_pool->task_arr[temp_pool->task_add_pos].function = function; //add new task to array
    temp_pool->task_arr[temp_pool->task_add_pos].arg = in_person;
    temp_pool->task_add_pos++; //update task array index and task count
    temp_pool->task_count++;
    pthread_cond_signal(&temp_pool->new_task); //notify threads of new task
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

//END EXAMPLE-BASED METHODS*************************************************************************************

//NEW THREADPOOL IMPLEMENTATION WIP*****************************************************************************

void *keep_thread_alive_new(void *in_thread_arg) //keep thread in loop to check for new task
{
    thread_arg *temp_thread_arg = (thread_arg*) in_thread_arg;
    tpool_t *temp_pool = temp_thread_arg->in_pool;
    struct person *temp_person = temp_thread_arg->thread_person;

    //make a struct that has tpool and person to preserve pool mutex while also passing in the person array
    while(1)
    {
        pthread_mutex_lock(&temp_pool->task_mut);

        while(temp_pool->current_task_pos >= temp_pool->task_count) //index == max index!!!!
        {
            pthread_cond_wait(&temp_pool->new_task, &temp_pool->task_mut);
        }

        temp_pool->current_task_pos++;
        pthread_mutex_unlock(&temp_pool->task_mut);
        
        if ((temp_person[temp_pool->current_task_pos]).p_type == 0) //keep extern that iterates through arr, just an extern that does patients + docs
        {
            waitForPatients((void*) &temp_person[temp_pool->current_task_pos]);
        }
        else //keep extern that iterates through arr, just an extern that does patients + docs
        {
            leaveOrEnter((void*) &temp_person[temp_pool->current_task_pos]);
        }
    }
}

tpool_t *create_pool_new(int thread_count, int task_count, struct person *in_person) //largely the same as current implementation, but passes person array directly into keep alive instead
{
    tpool_t *in_pool; // init pool
    in_pool = malloc(sizeof(tpool_t));
    in_pool->thread_arr = malloc(thread_count * sizeof(pthread_t));
    in_pool->task_arr = malloc(task_count * sizeof(tpool_task_t));
    in_pool->task_count = task_count;
    in_pool->task_add_pos = 0;
    in_pool->current_task_pos = -1;

    thread_arg *in_thread_arg; //use new thread_arg that
    in_thread_arg = malloc(sizeof(thread_arg));
    in_thread_arg->in_pool = in_pool;
    in_thread_arg->thread_person = in_person;

    for(int i = 0; i < thread_count; i++) //init all threads
    {   
        pthread_create(&in_pool->thread_arr[i], NULL, keep_thread_alive, (void *) in_thread_arg);
    }

    return in_pool;
}

//END NEW THREADPOOL IMPLEMENTATION WIP*************************************************************************