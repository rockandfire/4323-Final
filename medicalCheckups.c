/**
 * @file medicalCheckups.c
 * @author Cameron Stalcup (cameron.stalcup@okstate.edu)
 * @brief Group C
 * @date 04/25/22
 */

#include "threadpool.h"

int workAroundRealDocNumber3 = 0;
int workAroundRealpatNumber3 = 55;

void getMedicalCheckup(void *arg){
    // Locks mutex
    pthread_mutex_lock(&GET_CHECKUP);

    struct person *persons1 = (struct person*)arg;

    // If person is a doctor
    if(persons1->num < persons1->num_of_doctor){
        pthread_cond_signal(&DOCTOR_AVAILABLE);
        sem_wait(&SEM_CHECKUP);
        workAroundRealDocNumber3 = persons1->num;
    }

    // If person is a patient
    else{
        workAroundRealpatNumber3 = persons1->num - persons1->num_of_doctor;
        persons1->seeing_doc_num = workAroundRealDocNumber3;
        printf("Medical Professional %d (Thread ID: %d): Checking Patient %d \n", (workAroundRealDocNumber3), gettid(), workAroundRealpatNumber3);
        sem_post(&SEM_CHECKUP);
    }

    // Unlocks and locks back to keep counter1 function safe
    pthread_mutex_unlock(&GET_CHECKUP);
    performMedicalCheckup(persons1);
}

void performMedicalCheckup(void *arg){
    struct person *persons1 = (struct person*)arg;

    // If person is a patient
    if(persons1->num > persons1->num_of_doctor-1){
        printf("Patient %d (Thread ID: %d):  Getting Checkup \n", (persons1->num-persons1->num_of_doctor), gettid());
    }

    // If person is a doctor
    else{}

    // Waits for amount of time specified on input
    usleep(inputs.pat_check_time*1000);
    // Unlocks and locks back to keep counter1 function safe
    makePayment(persons1);
}
