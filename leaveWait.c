/**
 * @file leaveWait.c
 * @author Jackson Wildman (jawildm@okstate.edu)
 * @brief Group C
 * @date 04/25/22
 */

#include "threadpool.h"

void leaveOrEnter(void *arg) //decides whether a patient should leave without a checkup or enter the waiting room depending on whether the SEM_WAITING_ROOM room is full or not
{

    struct person *patient = (struct person*)arg;
    printf("Patient %d (Thread ID: %d): Arriving at the clinic front door \n",patient->num - inputs.num_of_med_prof, gettid());
    
    if(num_of_people_waiting >= inputs.wait_room_cap) //leave if no room in waiting room
    {  
        pthread_mutex_lock(&DATA_ENTRY); //ensure correct iteration of counter
        numOfPeopleThatLeft ++;
        pthread_mutex_unlock(&DATA_ENTRY);
        printf("Patient %d (Thread ID: %d): Leaving the clinic without checkup \n",patient->num - inputs.num_of_med_prof, gettid());
        pthread_cond_signal(&ALL_DONE); //allow main thread to check for finish condition
    }
    else //enter waiting room to find a sofa or stand
    {
        enterWaitingRoom(patient);
    }
}

void leaveClinic(void *arg) //leaves the clinic after a checkup
{

    struct person *temp_person = (struct person*)arg; //init temp

    if(temp_person->num > temp_person->num_of_doctor-1) //allow patient to leave clinic
    {
        pthread_cond_wait(&NOTIF_LEAVE_WAIT ,&LEAVE_WAIT); //wait until payment is complete
        pthread_mutex_lock(&DATA_ENTRY); //ensure correct iteration of counter
        successfulCheckups++;
        pthread_mutex_unlock(&DATA_ENTRY);
        averageWaitTimeForPat += ((temp_person->stop.tv_sec - temp_person->start.tv_sec) * 1000000 + temp_person->stop.tv_usec - temp_person->start.tv_usec);
        printf("Patient %d Leaving the clinic after receiving checkup\n", (temp_person->num - temp_person->num_of_doctor));
        pthread_mutex_unlock(&LEAVE_WAIT); //allow patient to leave
        pthread_cond_signal(&ALL_DONE);
    }
    else //return doctor to wait state
    {
        pthread_mutex_unlock(&LEAVE_WAIT); //allow next person to enter method
        waitForPatients(temp_person); 
    }
}

void waitForPatients(void *arg) //medical professional begins waiting for a patient to work on
{

    pthread_mutex_lock(&WAIT_FOR_PATIENT); //lock function
    doctCounter++;

    /** makes passed struct printf needs to be one line*/
    struct person *doctor = (struct person*)arg;
    gettimeofday(&(doctor->start), NULL);

    printf("Medical Professional %d (Thread ID: %d): Waiting for patient  \n", (doctor->num), gettid());

    if(num_of_people_sofa == 0) //wait for next patient
    {
        pthread_cond_wait(&PATIENT_READY ,&WAIT_FOR_PATIENT);
    }
    
    if(MainDone != 1) //continue if main method hasn't run out of patients
    {
        pthread_mutex_unlock(&WAIT_FOR_PATIENT);
        gettimeofday(&(doctor->stop), NULL);
        averageWaitTimeForDoc += ((doctor->stop.tv_sec - doctor->start.tv_sec) * 1000000 + doctor->stop.tv_usec - doctor->start.tv_usec); //update avg wait time
        getMedicalCheckup(doctor);
    }

    pthread_mutex_unlock(&WAIT_FOR_PATIENT); //guarantee unlock for next doc, prevents deadlock
}
