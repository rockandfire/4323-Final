#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/time.h>
#include "threadpool.h"

pid_t tid10;

void leaving_no_checkUp(void *arg){


    /** gets thread id*/
    tid10 = gettid();

    /** sets up passed struct*/
    struct person *persons1 = (struct person*)arg;
    //sem_wait(&sem);
    printf("Patient %d (Thread ID: %d): Arriving the clinic \n",persons1->num - inputs.num_of_med_prof, tid10);
    //sem_post(&sem);

    /**maximum arrival interval*/
    usleep(inputs.max_arr_time*1000);

    //printf("people wait: %d\n", num_of_poeple_waiting);
    if(num_of_poeple_waiting >= inputs.wait_room_cap){
        numOfPeopleThatLeft ++;
        persons1->made_it_through = 0;
        /** gets thread id*/
        tid10 = gettid();
        //sem_wait(&sem);
        printf("Patient %d (Thread ID: %d): Leaving the clinic without checkup \n",persons1->num - inputs.num_of_med_prof, tid10);
        //sem_post(&sem);
            while(1){
                //printf("\n");
            }
        }
    else{
        enterWaitingRoom(persons1);
    }
}

void leaveClinic(void *arg){

    struct person *persons1 = (struct person*)arg;

    /** if pat enter*/
    if(persons1->num > persons1->num_of_doctor-1){
        pthread_cond_wait(&notify6 ,&lock7);
        sem_wait(&sem);
        successfulCheckups++;
        persons1->made_it_through = 1;
        averageWaitTimeForPat += ((persons1->stop.tv_sec - persons1->start.tv_sec) * 1000000 + persons1->stop.tv_usec - persons1->start.tv_usec);
        printf("Patient %d Leaving the clinic after receiving checkup\n", (persons1->num-persons1->num_of_doctor));
        sem_post(&sem);
        }
    else{
        /** doctor go back to start*/
        pthread_mutex_unlock(&lock7);
        WaitForPatients(persons1);
    }

    /** unlock for pat to finish*/
    pthread_mutex_unlock(&lock7);
}

pid_t tid13;

void WaitForPatients(void *arg){

    /** locks func*/
     pthread_mutex_lock(&lock3);
     doctCounter++;

    /** makes passed struct printf needs to be one line*/
    struct person *persons1 = (struct person*)arg;
    gettimeofday(&(persons1->start), NULL);

    tid13 = gettid();
    printf("Medical Professional %d (Thread ID: %d): Waiting for patient  \n", (persons1->num), tid13 );

    /** waits for pat*/
    if(num_of_poeple_sofa==0){

        pthread_cond_wait(&notify3 ,&lock3);

    }
    if(MainDone != 1){
        /** unlocks and updates vars*/
        pthread_mutex_unlock(&lock3);
        gettimeofday(&(persons1->stop), NULL);
        averageWaitTimeForDoc += ((persons1->stop.tv_sec - persons1->start.tv_sec) * 1000000 + persons1->stop.tv_usec - persons1->start.tv_usec);
        getMedicalCheckup(persons1);
    }
    pthread_mutex_unlock(&lock3);
}