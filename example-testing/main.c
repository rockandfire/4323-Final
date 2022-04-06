#define THREAD 10
#define QUEUE  20

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include "threadpool.h"

int tasks = 0, done = 0;
pthread_mutex_t lock;
int doneCOunt = 0;
pthread_mutex_t lock1;
pthread_cond_t notify1;


pthread_mutex_t lock11;
pthread_cond_t notify11;

int num_of_poeple_waiting = 0;
int completed = 0;
int num_of_poeple_sofa = 0;
int count = 0;
int doctors_waiting = 0;
int num_of_doc_in_check =0;
int check_up_time = 0;


pthread_cond_t notifyMain;
pthread_mutex_t lockMain;

int main(int argc, char **argv){

    /** struct to handle inputs*/
    struct input inputs;
    inputs.num_of_med_prof = 4;
    inputs.num_of_pat = 100;
    inputs.wait_room_cap = 6;
    inputs.num_of_sofa = 3;
    inputs.max_arr_time = 10;
    inputs.pat_check_time = 1000;



    /** this comments code takes the input agrs and get separates them */
/*inputs.num_of_med_prof  = atoi(argv[1];
    inputs.num_of_pat       = atoi(argv[2];
    inputs.wait_room_cap    = atoi(argv[3];
    inputs.num_of_sofa      = atoi(argv[4];
    inputs.max_arr_time     = atoi(argv[5];
    inputs.pat_check_time   = atoi(argv[6];

    if (argc != 7){
        printf("Not enough args exiting");
        exit(0);
    }*/


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
    ending213 = inputs.num_of_med_prof;
    check_up_time = inputs.pat_check_time;

    /** mutex init*/
    pthread_mutex_init(&lock, NULL);


    /** thread pool struct creation
    Args: thread size, queue size(not used) and flags(not used)*/
    threadpool_t *pool1;
    assert((pool1 = threadpool_create(inputs.num_of_med_prof+inputs.wait_room_cap+5, inputs.num_of_pat-1, 0)) != NULL);

    /** add the doctors to the main functions
    args: pool struct, function, and the struct for the thread to be passed to keep track of vars*/
    for(int i = 0; i < inputs.num_of_med_prof; i++){
        threadpool_add(pool1, &ForPatients, &persons[i], 0);
        count++;
    }

    /** while less then 100 pat have went through the hospital
    needs to be changed to input struct pat size */
    while(count <= 100){

        /** delay counter for people coming and waiting at door */
        usleep(inputs.max_arr_time*1000);

        /** if there is room in waiting room send the next person*/
        if(num_of_poeple_waiting <inputs.wait_room_cap){

            /** adds next person to waitingroom with their struct (struct tells them what num person they are)*/
            threadpool_add(pool1, &enterWaitingRoom, &persons[count], 0);
            count++;
            }
        else{
            /** if no room after wait time leave*/
            threadpool_add(pool1, &leaving_no_checkUp, &persons[count], 0);
            }
        }



    printf("Done, now ending\n");

    /** FREES pool*/
    assert(threadpool_destroy(pool1, 0) == 0);

    fprintf(stderr, "Did %d tasks\n", done);

    return 0;
}
/** used but messy vars*/
int count6 = 0;

pthread_mutex_t lock2;
pthread_cond_t notify2;
int count1 = 0;
int waiting = 0;


void leaving_no_checkUp(void *arg){

    /** gets thread id*/
    pid_t tid = gettid();

    /** sets up passed struct*/
    struct person *persons1 = (struct person*)arg;
    printf("Patient %d (Thread ID: %d): Leaving the clinic without checkup \n",persons1->num , tid);

}

void enterWaitingRoom(void *arg){
    num_of_poeple_waiting ++;

    /** sets up passed struct*/
    struct person *persons1 = (struct person*)arg;

    /** prints the patients number ( minues the amount of doctors or else it wouldn't start at 0)*/
    printf("Patient %d ", (persons1->num-persons1->num_of_doctor));

    /** gets thread id*/
    pid_t tid = gettid();
    printf("(Thread ID: %d", (tid));
    printf("): Arriving the clinic \n", (tid));

    /**locks function*/
    pthread_mutex_lock(&lock2);

    /**while there is no room on sofa wait until notified*/
    while(num_of_poeple_sofa >= persons1->num_of_sofa){
        pthread_cond_wait(&notify2 ,&lock2);
    }

    count1++;

    /** broadcast notify2 */
    pthread_cond_broadcast(&notify2);

    /** unlock function*/
    pthread_mutex_unlock(&lock2);

    /** go to next function*/
    sitOnSofa(persons1);

}

/** used but messy vars*/
pthread_mutex_t lock3;
pthread_cond_t notify3;
int count2 = 0;
void sitOnSofa(void *arg){


   /** updates number of people on sofa*//
    num_of_poeple_sofa++;

    /** sets up passed struct*/
    struct person *persons1 = (struct person*)arg;

     /** prints the patients number*/
    printf("Patient %d ", (persons1->num-persons1->num_of_doctor));

    /** get thread id*/
    pid_t tid = gettid();

    /** printf needs to be made into 1 statment *//
    printf("(Thread ID: %d", (tid));
    printf("): Sitting on a sofa in the waiting room \n", (tid));

    /** locks func*/
    pthread_mutex_lock(&lock3);

        /** signal notify11 not broadcast  signals doc someone is ready*/
        pthread_cond_signal(&notify11);

        /** wait until notify3 goes off*/
        pthread_cond_wait(&notify3 ,&lock3);

    count2++;
    /** not used*/
    if (count2 >= (persons1->num_of_doctor) ){
        count2 = 0;
    }

    /** unlock func and update vars and leave */

    pthread_mutex_unlock(&lock3);
    num_of_poeple_sofa --;
    num_of_poeple_waiting --;
    getMedicalCheckup(persons1);
}



/** used but messy vars*/
pthread_mutex_t lock13;
pthread_cond_t notify13;

void ForPatients(void *arg){

    /** locks func*/
     pthread_mutex_lock(&lock11);

    /** updates how many docs are waiting*/
    doctors_waiting ++;

    /** makes passed struct printf needs to be one line*/
    struct person *persons1 = (struct person*)arg;
    printf("Medical Professional  %d ", (persons1->num));
    pid_t tid = gettid();
    printf("(Thread ID: %d", (tid));
    printf("): Waiting for patient  \n");

    /** waits for pat*/
        pthread_cond_wait(&notify11 ,&lock11);

    /** unlocks and updates vars*/
    pthread_mutex_unlock(&lock11);
    getMedicalCheckup(persons1);
    doctors_waiting --;

}
/** used but messy vars*/
pthread_mutex_t lock5;
pthread_mutex_t lock4;
pthread_cond_t notify4;
int count3 = 0;
void getMedicalCheckup(void *arg){

    /** locks */
    pthread_mutex_lock(&lock4);

    struct person *persons1 = (struct person*)arg;

    /** if your a doctor enter if loop*/
    if(persons1->num < persons1->num_of_doctor){
        num_of_doc_in_check++;
        pthread_cond_signal(&notify3);
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

/** used but messy vars*/
pthread_cond_t notify5;

pthread_mutex_t lock15;
pthread_cond_t notify15;

pthread_mutex_t lock16;
pthread_cond_t notify16;

pthread_mutex_t lock100;
pthread_cond_t notify100;


int count4 = 0;
void perfromMedicalCheckup(void *arg){

    struct person *persons1 = (struct person*)arg;

    /** if your a patient enter if loop*/
    if(persons1->num > persons1->num_of_doctor-1){
        pid_t tid = gettid();
        printf("Patient %d (Thread ID: %d ):  Getting checkup \n", (persons1->num-persons1->num_of_doctor), tid);
    }
    else{

    }

    /**unlocks and locks back to keep next function safe*/
    pthread_mutex_unlock(&lock5);
    pthread_mutex_lock(&lock100);
    makePayment(persons1);
}
/** used but messy vars*/
pthread_mutex_t lock6;
pthread_cond_t notify6;
int count5 = 0;


pthread_mutex_t lock8;
pthread_cond_t notify8;

pthread_mutex_t lock101;
pthread_cond_t notify101;

pthread_mutex_t lock102;
pthread_cond_t notify102;

pthread_cond_t notify103;
int count7 = 0;

void makePayment(void *arg){

    struct person *persons1 = (struct person*)arg;

    /** if your a patienit enter if loop*/
    if(persons1->num > persons1->num_of_doctor-1){

        pid_t tid = gettid();

        printf("Patient %d (Thread ID: %d ): Making Payment to Medical Professional  ", (persons1->num-persons1->num_of_doctor), tid);

        /** nofity doc to print*/
        pthread_cond_signal(&notify16);

    }
    /** if your a doctor enter here*/
    else{
        /** wait for pat to tell you to print so you dont print before them*/
        pthread_cond_wait(&notify16 ,&lock100);

        printf(" %d \n", (persons1->num));

        /** signal next print out that its okay to print*/
        pthread_cond_signal(&notify102);
    }

    /**unlocks and locks back to keep next function safe*/
    pthread_mutex_unlock(&lock100);
     pthread_mutex_lock(&lock101);
    acceptPayment(persons1);
}
/** used but messy vars*/
pthread_cond_t notify104;
pthread_mutex_t lock9;
pthread_cond_t notify9;
pthread_mutex_t lock103;
int count8 = 0;
void acceptPayment(void *arg){

    struct person *persons1 = (struct person*)arg;

    /** if your a patienit enter if loop*/
    if(persons1->num > persons1->num_of_doctor-1){

        pid_t tid = gettid();
         pthread_cond_wait(&notify102 ,&lock101);

        /** print out is wrong*/
        printf("Patient2 %d (Thread ID: %d ): apptect  Professional  ", (persons1->num-persons1->num_of_doctor), tid);

        /** notify next print*/
        pthread_cond_signal(&notify9);
    }
    else{
        /** wait to print*/
        pthread_cond_wait(&notify9 ,&lock101);
        printf(" %d \n", (persons1->num));

        /** notify next print*/
        pthread_cond_signal(&notify104);
    }

    pthread_mutex_unlock(&lock101);
    pthread_mutex_lock(&lock103);;
    leaveClinic(persons1);
}

void leaveClinic(void *arg){

    struct person *persons1 = (struct person*)arg;

    doneCOunt++;

    /** if pat enter*/
    if(persons1->num > persons1->num_of_doctor-1){
        pthread_cond_wait(&notify104 ,&lock103);
        printf("Patient %d has left with check up\n", (persons1->num-persons1->num_of_doctor));
        }
    else{
        /** doctor go back to start*/
        pthread_mutex_unlock(&lock103);
        ForPatients(persons1);
    }

    /** unlock for pat to finish*/
    pthread_mutex_unlock(&lock103);

}
