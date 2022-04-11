#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/time.h>
#include "threadpool.h"

pthread_mutex_t lock10;
pthread_cond_t notify10;
int wait = 0;
int workAroundRealpatNumber2print = 0;
int workAroundRealDocNumber2 = 0;
int workAroundRealpatNumber2 = 55;

pid_t tid2;

void makePayment(void *arg){

    struct person *persons1 = (struct person*)arg;

    /** if your a patienit enter if loop*/
    if(persons1->num > persons1->num_of_doctor-1){

        sem_wait(&sem3);

        workAroundRealpatNumber2 = persons1->seeing_doc_num;
        workAroundRealpatNumber2print = persons1->num-persons1->num_of_doctor;
        tid2 = gettid();

        wait = 1;
        pthread_cond_wait(&notify10 ,&lock10);
        pthread_mutex_unlock(&lock10);
        wait = 0;

        sem_post(&sem3);
    }
    /** if your a doctor enter here*/
    else{
        while(workAroundRealpatNumber2 != persons1->num){}
        while(wait ==0){}
        workAroundRealDocNumber2=persons1->num;
        printf("Patient %d (Thread ID: %d): Making Payment to Medical Professional %d \n", (workAroundRealpatNumber2print), tid2, workAroundRealDocNumber2);
        pthread_cond_signal(&notify10);
    }

    //pthread_mutex_lock(&lock7);
    acceptPayment(persons1);
}

pthread_mutex_t lock7;
pthread_cond_t notify8;

int workAroundRealDocNumber = 0;
int workAroundRealpatNumber = 0;
int tid1 = 0;

void acceptPayment(void *arg){

    struct person *persons1 = (struct person*)arg;

    /** if your a patienit enter if loop*/
    if(persons1->num > persons1->num_of_doctor-1){


        //tid1 = tid;
         pthread_cond_wait(&notify8 ,&lock7);
        sem_wait(&sem);

        /** notify counter1 print*/
        workAroundRealpatNumber = persons1->num-persons1->num_of_doctor;
        pthread_cond_signal(&notify5);
    }
    else{
        /** wait to print*/
        pthread_cond_signal(&notify8);
        pthread_cond_wait(&notify5 ,&lock7);

        workAroundRealDocNumber=persons1->num;
        tid1 = gettid();
        printf("Medical Professional %d (Thread ID: %d): Accepting Payment from Patient %d \n", (workAroundRealDocNumber), tid1, workAroundRealpatNumber);
        /** notify counter1 print*/
        pthread_cond_signal(&notify6);
        sem_post(&sem);
    }

    pthread_mutex_unlock(&lock7);
    pthread_mutex_lock(&lock7);;
    leaveClinic(persons1);
}