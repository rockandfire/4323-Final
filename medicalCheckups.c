#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/time.h>
#include "threadpool.h"

pid_t tid3;
int workAroundRealDocNumber3 = 0;
int workAroundRealpatNumber3 = 55;

void getMedicalCheckup(void *arg){

    /** locks */
    pthread_mutex_lock(&lock4);

    struct person *persons1 = (struct person*)arg;

    /** if your a doctor enter if loop*/
    if(persons1->num < persons1->num_of_doctor){
        pthread_cond_signal(&notify2);
        tid3 = gettid();
        sem_wait(&sem);

        workAroundRealDocNumber3 = persons1->num;
    }
    /** if your not a a doctor come here*/
    else{
        workAroundRealpatNumber3 = persons1->num-persons1->num_of_doctor;

        persons1->seeing_doc_num = workAroundRealDocNumber3;
        printf("Medical Professional %d (Thread ID: %d): Checking Patient %d \n", (workAroundRealDocNumber3), tid3, workAroundRealpatNumber3);
        sem_post(&sem);
    }

    /**unlocks and locks back to keep counter1 function safe*/
    pthread_mutex_unlock(&lock4);
   // pthread_mutex_lock(&lock5);
    perfromMedicalCheckup(persons1);
}
pid_t tid14;
void perfromMedicalCheckup(void *arg){

    struct person *persons1 = (struct person*)arg;

    /** if your a patient enter if loop*/
    if(persons1->num > persons1->num_of_doctor-1){
        tid14 = gettid();
        printf("Patient %d (Thread ID: %d):  Getting checkup \n", (persons1->num-persons1->num_of_doctor), tid14);
    }
    else{

    }

    usleep(inputs.pat_check_time*1000);
    /**unlocks and locks back to keep counter1 function safe*/
    makePayment(persons1);
}