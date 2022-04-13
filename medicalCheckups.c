//cameron stuff

#include "threadpool.h"

pid_t tid3;
int workAroundRealDocNumber3 = 0;
int workAroundRealpatNumber3 = 55;

void getMedicalCheckup(void *arg){
    // Locks mutex
    pthread_mutex_lock(&lock4);

    struct person *persons1 = (struct person*)arg;

    // If person is a doctor
    if(persons1->num < persons1->num_of_doctor){
        pthread_cond_signal(&notify2);
        tid3 = gettid();
        sem_wait(&sem);
        workAroundRealDocNumber3 = persons1->num;
    }

    // If person is a patient
    else{
        workAroundRealpatNumber3 = persons1->num - persons1->num_of_doctor;
        persons1->seeing_doc_num = workAroundRealDocNumber3;
        printf("Medical Professional %d (Thread ID: %d): Checking Patient %d \n", (workAroundRealDocNumber3), tid3, workAroundRealpatNumber3);
        sem_post(&sem);
    }

    // Unlocks and locks back to keep counter1 function safe
    pthread_mutex_unlock(&lock4);
    // pthread_mutex_lock(&lock5);
    performMedicalCheckup(persons1);
}

pid_t tid14;
void performMedicalCheckup(void *arg){
    struct person *persons1 = (struct person*)arg;

    // If person is a patient
    if(persons1->num > persons1->num_of_doctor-1){
        tid14 = gettid();
        printf("Patient %d (Thread ID: %d):  Getting Checkup \n", (persons1->num-persons1->num_of_doctor), tid14);
    }

    // If person is a doctor
    else{}

    // Waits for amount of time specified on input
    usleep(inputs.pat_check_time*1000);
    // Unlocks and locks back to keep counter1 function safe
    makePayment(persons1);
}
