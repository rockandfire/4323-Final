//jackson stuff

#include "threadpool.h"

pid_t tid_patient;

void leaving_no_checkUp(void *arg){


    /** gets thread id*/
    tid_patient = gettid();

    /** sets up passed struct*/
    struct person *patient = (struct person*)arg;
    //sem_wait(&sem);
    printf("Patient %d (Thread ID: %d): Arriving the clinic \n",patient->num - inputs.num_of_med_prof, tid_patient);
    //sem_post(&sem);

    /**maximum arrival interval*/
    usleep(inputs.max_arr_time*1000);

    //printf("people wait: %d\n", num_of_poeple_waiting);
    if(num_of_poeple_waiting >= inputs.wait_room_cap){
        numOfPeopleThatLeft ++;
        patient->made_it_through = 0;
        /** gets thread id*/
        tid_patient = gettid();
        //sem_wait(&sem);
        printf("Patient %d (Thread ID: %d): Leaving the clinic without checkup \n",patient->num - inputs.num_of_med_prof, tid_patient);
        //sem_post(&sem);
            while(1){
                //printf("\n");
            }
        }
    else{
        enterWaitingRoom(patient);
    }
}

void leaveClinic(void *arg){

    struct person *patient = (struct person*)arg;

    /** if pat enter*/
    if(patient->num > patient->num_of_doctor-1){
        pthread_cond_wait(&notify6 ,&lock7);
        sem_wait(&sem);
        successfulCheckups++;
        patient->made_it_through = 1;
        averageWaitTimeForPat += ((patient->stop.tv_sec - patient->start.tv_sec) * 1000000 + patient->stop.tv_usec - patient->start.tv_usec);
        printf("Patient %d Leaving the clinic after receiving checkup\n", (patient->num - patient->num_of_doctor));
        sem_post(&sem);
        }
    else{
        /** doctor go back to start*/
        pthread_mutex_unlock(&lock7);
        WaitForPatients(patient);
    }

    /** unlock for pat to finish*/
    pthread_mutex_unlock(&lock7);
}

pid_t tid_doctor;

void WaitForPatients(void *arg){

    /** locks func*/
     pthread_mutex_lock(&lock3);
     doctCounter++;

    /** makes passed struct printf needs to be one line*/
    struct person *doctor = (struct person*)arg;
    gettimeofday(&(doctor->start), NULL);

    tid_doctor = gettid();
    printf("Medical Professional %d (Thread ID: %d): Waiting for patient  \n", (doctor->num), tid_doctor);

    /** waits for pat*/
    if(num_of_poeple_sofa==0){

        pthread_cond_wait(&notify3 ,&lock3);

    }
    if(MainDone != 1){
        /** unlocks and updates vars*/
        pthread_mutex_unlock(&lock3);
        gettimeofday(&(doctor->stop), NULL);
        averageWaitTimeForDoc += ((doctor->stop.tv_sec - doctor->start.tv_sec) * 1000000 + doctor->stop.tv_usec - doctor->start.tv_usec);
        getMedicalCheckup(doctor);
    }
    pthread_mutex_unlock(&lock3);
}