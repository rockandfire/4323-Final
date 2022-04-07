#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include "threadpool.h"

int num_of_poeple_waiting = 0;
int num_of_poeple_sofa = 0;
int count = 0;
int check_up_time = 0;
int MainDone = 0;

int main(int argc, char **argv){

    /** struct to handle inputs*/
    if (argc == 7){
        //printf("Not enough args exiting");
        //exit(0);
        inputs.num_of_med_prof  = atoi(argv[1]);
        inputs.num_of_pat       = atoi(argv[2]);
        inputs.wait_room_cap    = atoi(argv[3]);
        inputs.num_of_sofa      = atoi(argv[4]);
        inputs.max_arr_time     = atoi(argv[5]);
        inputs.pat_check_time   = atoi(argv[6]);
    }
    else{
        inputs.num_of_med_prof = 2;
        inputs.num_of_pat = 50;
        inputs.wait_room_cap = 3;
        inputs.num_of_sofa = 2;
        inputs.max_arr_time = 10;
        inputs.pat_check_time = 100;
    }

    /** struct to be passed around with threads*/
    struct person persons[120];

    /** initializes struct*/
    for (int i =0; i<120; i++){
        persons[i].num = i;
        persons[i].done = 1;
        persons[i].num_of_pat_left = inputs.num_of_pat;
        persons[i].num_of_doctor = inputs.num_of_med_prof;
        persons[i].num_of_sofa = inputs.num_of_sofa;
    }
    /** vars used in functions needs cleaned up*/
    check_up_time = inputs.pat_check_time;

    /** mutex init*/
    pthread_mutex_init(&lock, NULL);

    /** thread pool struct creation
    Args: thread size, queue size(not used) and flags(not used)*/
    struct threadpool_t *pool1;
    assert((pool1 = threadpool_create(inputs.num_of_med_prof+inputs.wait_room_cap+5, inputs.num_of_pat-1)) != NULL);

    /** add the doctors to the main functions
    args: pool struct, function, and the struct for the thread to be passed to keep track of vars*/
    for(int i = 0; i < inputs.num_of_med_prof; i++){
        threadpool_add(pool1, &ForPatients, &persons[i]);
        count++;
    }

    /** while less then 100 pat have went through the hospital
    needs to be changed to input struct pat size */
    while(count <= inputs.num_of_pat + inputs.num_of_med_prof){

        /** delay counter for people coming and waiting at door */
        usleep(inputs.max_arr_time*1000);

        /** adds next person to waitingroom with their struct (struct tells them what num person they are)*/
        threadpool_add(pool1, &leaving_no_checkUp, &persons[count]);
        count++;
        }

    while(num_of_poeple_sofa != 0 && count != inputs.num_of_pat + inputs.num_of_med_prof){
    }

    MainDone = 1;
    sleep(1);
    pthread_cond_broadcast(&notify3);
    usleep(10);
    printf("Done, now ending\n");

    /** FREES pool*/
    assert(threadpool_destroy(pool1) == 0);

    return 0;
}

void leaving_no_checkUp(void *arg){

    /** gets thread id*/
    pid_t tid = gettid();

    /** sets up passed struct*/
    struct person *persons1 = (struct person*)arg;

    printf("Patient %d (Thread ID: %d): Arriving the clinic \n",persons1->num - inputs.num_of_med_prof, tid);

    /**maximum arrival interval*/
    usleep(inputs.max_arr_time*1000);

    if(num_of_poeple_waiting >inputs.wait_room_cap){
        /** gets thread id*/
        pid_t tid = gettid();
        printf("Patient %d (Thread ID: %d): Leaving the clinic without checkup \n",persons1->num - inputs.num_of_med_prof, tid);
        }
    else{
        enterWaitingRoom(persons1);
    }
}

void enterWaitingRoom(void *arg){
    num_of_poeple_waiting ++;

    /** sets up passed struct*/
    struct person *persons1 = (struct person*)arg;

    /** prints the patients number ( minues the amount of doctors or else it wouldn't start at 0)*/
    pid_t tid = gettid();
    printf("Patient %d (Thread ID: %d): Arriving the clinic\n", (persons1->num-persons1->num_of_doctor),tid);

    /**locks function*/
    pthread_mutex_lock(&lock1);

    /**while there is no room on sofa wait until notified*/
    while(num_of_poeple_sofa >= persons1->num_of_sofa){
        pthread_cond_wait(&notify1 ,&lock1);
    }

    /** broadcast notify1 */
    pthread_cond_broadcast(&notify1);

    /** unlock function*/
    pthread_mutex_unlock(&lock1);

    /** go to next function*/
    sitOnSofa(persons1);

}

void sitOnSofa(void *arg){

   /** updates number of people on sofa*/
    num_of_poeple_sofa++;

    /** sets up passed struct*/
    struct person *persons1 = (struct person*)arg;

     /** prints the patients number, get thread id*/
    pid_t tid = gettid();
    printf("Patient %d (Thread ID: %d): Sitting on a sofa in the waiting room \n", (persons1->num-persons1->num_of_doctor), tid );

    /** locks func*/
    pthread_mutex_lock(&lock2);

    /** signal notify11 not broadcast  signals doc someone is ready*/
    pthread_cond_signal(&notify3);

    /** wait until notify2 goes off*/
    pthread_cond_wait(&notify2 ,&lock2);


    /** unlock func and update vars and leave */

    pthread_mutex_unlock(&lock2);
    num_of_poeple_sofa --;
    num_of_poeple_waiting --;
    getMedicalCheckup(persons1);
}

void ForPatients(void *arg){

    /** locks func*/
     pthread_mutex_lock(&lock3);

    /** makes passed struct printf needs to be one line*/
    struct person *persons1 = (struct person*)arg;
    pid_t tid = gettid();
    printf("Medical Professional  %d (Thread ID: %d): Waiting for patient  \n", (persons1->num), tid );

    /** waits for pat*/
    if(num_of_poeple_sofa==0){
        pthread_cond_wait(&notify3 ,&lock3);

        if(MainDone == 1){
            pthread_mutex_unlock(&lock3);
            docleave(persons1);
        }
    }
    if(MainDone != 1){
        /** unlocks and updates vars*/
        pthread_mutex_unlock(&lock3);

        getMedicalCheckup(persons1);
    }
    pthread_mutex_unlock(&lock3);
}
void docleave(void *arg){

}
void getMedicalCheckup(void *arg){

    /** locks */
    pthread_mutex_lock(&lock4);

    struct person *persons1 = (struct person*)arg;

    /** if your a doctor enter if loop*/
    if(persons1->num < persons1->num_of_doctor){
        pthread_cond_signal(&notify2);
        pid_t tid = gettid();
        printf("Medical Professional  %d (Thread ID: %d): Checking Patient ", (persons1->num), tid);
    }
    /** if your not a a doctor come here*/
    else{
        printf(" %d \n", (persons1->num-persons1->num_of_doctor));
    }

    /**unlocks and locks back to keep next function safe*/
    pthread_mutex_unlock(&lock4);
    pthread_mutex_lock(&lock5);
    perfromMedicalCheckup(persons1);
}

void perfromMedicalCheckup(void *arg){

    struct person *persons1 = (struct person*)arg;

    /** if your a patient enter if loop*/
    if(persons1->num > persons1->num_of_doctor-1){
        pid_t tid = gettid();
        printf("Patient %d (Thread ID: %d ):  Getting checkup \n", (persons1->num-persons1->num_of_doctor), tid);
    }
    else{

    }

    usleep(inputs.pat_check_time*1000);

    /**unlocks and locks back to keep next function safe*/
    pthread_mutex_unlock(&lock5);
    pthread_mutex_lock(&lock6);
    makePayment(persons1);
}

void makePayment(void *arg){

    struct person *persons1 = (struct person*)arg;

    /** if your a patienit enter if loop*/
    if(persons1->num > persons1->num_of_doctor-1){

        pid_t tid = gettid();

        printf("Patient %d (Thread ID: %d ): Making Payment to Medical Professional ", (persons1->num-persons1->num_of_doctor), tid);

        /** nofity doc to print*/
        pthread_cond_signal(&notify4);

    }
    /** if your a doctor enter here*/
    else{
        /** wait for pat to tell you to print so you dont print before them*/
        pthread_cond_wait(&notify4 ,&lock6);

        printf(" %d \n", (persons1->num));

        /** signal next print out that its okay to print*/
        pthread_cond_signal(&notify7);
    }

    /**unlocks and locks back to keep next function safe*/
    pthread_mutex_unlock(&lock6);
     pthread_mutex_lock(&lock7);
    acceptPayment(persons1);
}

void acceptPayment(void *arg){

    struct person *persons1 = (struct person*)arg;

    /** if your a patienit enter if loop*/
    if(persons1->num > persons1->num_of_doctor-1){

        pid_t tid = gettid();
         pthread_cond_wait(&notify7 ,&lock7);

        printf("Patient %d (Thread ID: %d ): Accepting Payment from Patient ", (persons1->num-persons1->num_of_doctor), tid);

        /** notify next print*/
        pthread_cond_signal(&notify5);
    }
    else{
        /** wait to print*/
        pthread_cond_wait(&notify5 ,&lock7);
        printf(" %d \n", (persons1->num));

        /** notify next print*/
        pthread_cond_signal(&notify6);
    }

    pthread_mutex_unlock(&lock7);
    pthread_mutex_lock(&lock7);;
    leaveClinic(persons1);
}

void leaveClinic(void *arg){

    struct person *persons1 = (struct person*)arg;

    /** if pat enter*/
    if(persons1->num > persons1->num_of_doctor-1){
        pthread_cond_wait(&notify6 ,&lock7);
        printf("Patient %d Leaving the clinic after receiving checkup\n", (persons1->num-persons1->num_of_doctor));
        }
    else{
        /** doctor go back to start*/
        pthread_mutex_unlock(&lock7);
        ForPatients(persons1);
    }

    /** unlock for pat to finish*/
    pthread_mutex_unlock(&lock7);
}
