/**
 * @file waitingRoom.c
 * @author Josh Hardin (joshua.hardin@okstate.edu)
 * @brief Group C
 * @date 04/25/22
 */

#include "threadpool.h"


void enterWaitingRoom(void *arg){

    sem_wait(&SEM_WAITING_ROOM);
    num_of_people_waiting ++;
    sem_post(&SEM_WAITING_ROOM);


    /** sets up passed struct*/
    struct person *persons1 = (struct person*)arg;

    //gettimeofday(&stop, NULL);

    gettimeofday(&(persons1->start), NULL);

    //gettimeofday(&start, NULL);

    /** prints the patients number ( minues the amount of doctors or else it wouldn't start at 0)*/
    printf("Patient %d (Thread ID: %d): Entering the clinic\n", (persons1->num-persons1->num_of_doctor), gettid());

    /**locks function*/
    pthread_mutex_lock(&NEW_ENTRY);

    standing_queue.times[standing_queue.end] = persons1->start;
    standing_queue.end++;

    pthread_mutex_unlock(&NEW_ENTRY);

    while (num_of_people_sofa == persons1->num_of_sofa && (standing_queue.times[standing_queue.current].tv_sec != persons1->start.tv_sec))
    {
        pthread_cond_wait(&SOFA_OPEN, &NEW_ENTRY);
    }

    pthread_mutex_lock(&NEW_ENTRY);
    standing_queue.current++;
    pthread_mutex_unlock(&NEW_ENTRY);
    /** go to counter1 function*/
    sitOnSofa(persons1);

}

void sitOnSofa(void *arg){

   /** updates number of people on sofa*/
    sem_wait(&SEM_WAITING_ROOM);
    num_of_people_sofa++;
    sem_post(&SEM_WAITING_ROOM);

    /** sets up passed struct*/
    struct person *persons1 = (struct person*)arg;

     /** prints the patients number, get thread id*/
    printf("Patient %d (Thread ID: %d): Sitting on a sofa in the waiting room \n", (persons1->num-persons1->num_of_doctor), gettid());

    /** locks func*/
    //pthread_mutex_lock(&WAIT_FOR_DOC);

    /** signal notify11 not broadcast  signals doc someone is ready*/
    pthread_cond_signal(&PATIENT_READY);

    /** wait until DOCTOR_AVAILABLE goes off*/
    pthread_cond_wait(&DOCTOR_AVAILABLE ,&WAIT_FOR_DOC);


    /** unlock func and update vars and leave */

    pthread_mutex_unlock(&WAIT_FOR_DOC);
    sem_wait(&SEM_WAITING_ROOM);
    num_of_people_sofa --;
    num_of_people_waiting --;
    sem_post(&SEM_WAITING_ROOM);

    pthread_cond_signal(&SOFA_OPEN);
    gettimeofday(&(persons1->stop), NULL);
    getMedicalCheckup(persons1);
}