/**
 * @file tpool.c
 * @author Jackson Wildman (jawildm@okstate.edu)
 * @brief Group C
 * @date 04/12/22
 */

#include "threadpool.h"

pid_t tid_patient;
pid_t tid_doctor;

void leaveOrEnter(void *arg) //decides whether a patient should leave without a checkup or enter the waiting room depending on whether the waiting room is full or not
{

    tid_patient = gettid(); //get tid and init temp patient
    struct person *patient = (struct person*)arg;
    printf("Patient %d (Thread ID: %d): Arriving the clinic \n",patient->num - inputs.num_of_med_prof, tid_patient);

    usleep(inputs.max_arr_time*1000); //arrival interval

    if(num_of_poeple_waiting >= inputs.wait_room_cap) //leave if no room
    {
        numOfPeopleThatLeft ++;
        patient->made_it_through = 0;
        tid_patient = gettid();
        printf("Patient %d (Thread ID: %d): Leaving the clinic without checkup \n",patient->num - inputs.num_of_med_prof, tid_patient);
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
        pthread_cond_wait(&notify6 ,&lock7); //wait until payment is complete
        sem_wait(&sem); //prevent race condition
        successfulCheckups++;
        temp_person->made_it_through = 1;
        averageWaitTimeForPat += ((temp_person->stop.tv_sec - temp_person->start.tv_sec) * 1000000 + temp_person->stop.tv_usec - temp_person->start.tv_usec);
        printf("Patient %d Leaving the clinic after receiving checkup\n", (temp_person->num - temp_person->num_of_doctor));
        sem_post(&sem);
        pthread_mutex_unlock(&lock7); //allow patient to leave
    }
    else //return doctor to wait state
    {
        pthread_mutex_unlock(&lock7); //allow next person to enter method
        waitForPatients(temp_person); 
    }
}

void waitForPatients(void *arg) //medical professional begins waiting for a patient to work on
{

    pthread_mutex_lock(&lock3); //lock function
    doctCounter++;

    /** makes passed struct printf needs to be one line*/
    struct person *doctor = (struct person*)arg;
    gettimeofday(&(doctor->start), NULL);

    tid_doctor = gettid();
    printf("Medical Professional %d (Thread ID: %d): Waiting for patient  \n", (doctor->num), tid_doctor);

    if(num_of_poeple_sofa == 0) //wait for patient
    {
        pthread_cond_wait(&notify3 ,&lock3);
    }
    
    if(MainDone != 1) //continue if main method hasn't run out of patients
    {
        pthread_mutex_unlock(&lock3);
        gettimeofday(&(doctor->stop), NULL);
        averageWaitTimeForDoc += ((doctor->stop.tv_sec - doctor->start.tv_sec) * 1000000 + doctor->stop.tv_usec - doctor->start.tv_usec); //update avg wait time
        getMedicalCheckup(doctor);
    }

    pthread_mutex_unlock(&lock3); //guarantee unlock for next doc
}