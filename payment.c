/**
 * @file payment.c
 * @author Samuel Fipps (sfipps@okstate.edu)
 * @brief Group C
 * @date 04/25/22
 */

#include "threadpool.h"

int wait = 0; 
int workAroundRealpatNumber2print = 0;
int workAroundRealDocNumber2 = 0;
int workAroundRealpatNumber2 = 0;

void makePayment(void *arg){

    struct person *persons1 = (struct person*)arg;

    /** if your a patienit enter if loop*/
    if(persons1->num > persons1->num_of_doctor-1){

        sem_wait(&SEM_MAKE_PAYMENT);

        workAroundRealpatNumber2 = persons1->seeing_doc_num;
        workAroundRealpatNumber2print = persons1->num-persons1->num_of_doctor;

        wait = 1;
        pthread_cond_wait(&MAKING_PAYMENT ,&LOCK_MAKE_PAYMENT);
        pthread_mutex_unlock(&LOCK_MAKE_PAYMENT);
        wait = 0;

        sem_post(&SEM_MAKE_PAYMENT);
    }
    /** if your a doctor enter here*/
    else{
        while(workAroundRealpatNumber2 != persons1->num){}
        while(wait == 0){}
        workAroundRealDocNumber2=persons1->num;
        printf("Patient %d (Thread ID: %d): Making Payment to Medical Professional %d \n", (workAroundRealpatNumber2print), gettid(), workAroundRealDocNumber2);
        pthread_cond_signal(&MAKING_PAYMENT);
    }

    acceptPayment(persons1);
}

int workAroundRealDocNumber = 0;
int workAroundRealpatNumber = 0;

void acceptPayment(void *arg){

    struct person *persons1 = (struct person*)arg;

    /** if your a patienit enter if loop*/
    if(persons1->num > persons1->num_of_doctor-1){


        pthread_cond_wait(&PAYMENT_ACCEPTED ,&LEAVE_WAIT);
        sem_wait(&SEM_PAYMENT_ACCEPT);

        /** notify counter1 print*/
        workAroundRealpatNumber = persons1->num-persons1->num_of_doctor;
        pthread_cond_signal(&PAYMENT_COMPLETE);
    }
    else{
        /** wait to print*/
        pthread_cond_signal(&PAYMENT_ACCEPTED);
        pthread_cond_wait(&PAYMENT_COMPLETE ,&LEAVE_WAIT);

        workAroundRealDocNumber=persons1->num;
        printf("Medical Professional %d (Thread ID: %d): Accepting Payment from Patient %d \n", (workAroundRealDocNumber), gettid(), workAroundRealpatNumber);
        /** notify counter1 print*/
        pthread_cond_signal(&NOTIF_LEAVE_WAIT);
        sem_post(&SEM_PAYMENT_ACCEPT);
    }
    
    leaveClinic(persons1);
}