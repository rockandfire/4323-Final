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

    //printf("hi");
    struct input inputs;
    inputs.num_of_med_prof = 4;
    inputs.num_of_pat = 100;
    inputs.wait_room_cap = 6;
    inputs.num_of_sofa = 3;
    inputs.max_arr_time = 10;
    inputs.pat_check_time = 1000;

    check_up_time = inputs.pat_check_time;
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



    struct person persons[120];

    for (int i =0; i<120; i++){
        persons[i].num = i;
        persons[i].done = 1;
        persons[i].num_of_pat_left = inputs.num_of_pat;
        persons[i].num_of_doctor = inputs.num_of_med_prof;
        persons[i].num_of_sofa = inputs.num_of_sofa;
    }
    ending213 = inputs.num_of_med_prof;
    pthread_mutex_init(&lock, NULL);

    threadpool_t *pool1;
    assert((pool1 = threadpool_create(inputs.num_of_med_prof+inputs.wait_room_cap+5, inputs.num_of_pat-1, 0)) != NULL);

    for(int i = 0; i < inputs.num_of_med_prof; i++){
        threadpool_add(pool1, &ForPatients, &persons[i], 0);
        count++;
    }

    while(count <= 100){
        usleep(inputs.max_arr_time*1000);
        if(num_of_poeple_waiting <inputs.wait_room_cap){
            threadpool_add(pool1, &enterWaitingRoom, &persons[count], 0);
            count++;
            }
        else{
            threadpool_add(pool1, &leaving_no_checkUp, &persons[count], 0);
            }
        }


    //pthread_cond_wait(&notifyMain ,&lockMain);
    //pthread_mutex_unlock(&lockMain);
    printf("Done, now ending\n");

    assert(threadpool_destroy(pool1, 0) == 0);

    fprintf(stderr, "Did %d tasks\n", done);

    return 0;
}

int count6 = 0;

pthread_mutex_t lock2;
pthread_cond_t notify2;

int count1 = 0;
int waiting = 0;

void leaving_no_checkUp(void *arg){
    //Patient 9 (Thread ID: XXXXXX): Leaving the clinic without checkup
    pid_t tid = gettid();
    struct person *persons1 = (struct person*)arg;
    printf("Patient %d (Thread ID: %d): Leaving the clinic without checkup \n",persons1->num , tid);

}

void enterWaitingRoom(void *arg){
    num_of_poeple_waiting ++;

    //pthread_mutex_lock(&lock2);
    struct person *persons1 = (struct person*)arg;
    printf("Patient %d ", (persons1->num-persons1->num_of_doctor));
    pid_t tid = gettid();
    printf("(Thread ID: %d", (tid));
    printf("): Arriving the clinic \n", (tid));

    pthread_mutex_lock(&lock2);
    while(num_of_poeple_sofa >= persons1->num_of_sofa){
        pthread_cond_wait(&notify2 ,&lock2);
    }

    count1++;
    pthread_cond_broadcast(&notify2);
    pthread_mutex_unlock(&lock2);


    sitOnSofa(persons1);

}


pthread_mutex_t lock3;
pthread_cond_t notify3;
int count2 = 0;
void sitOnSofa(void *arg){

   // printf("hidasdas\n");

    num_of_poeple_sofa++;

    struct person *persons1 = (struct person*)arg;
    printf("Patient %d ", (persons1->num-persons1->num_of_doctor));
    pid_t tid = gettid();
    printf("(Thread ID: %d", (tid));
    printf("): Sitting on a sofa in the waiting room \n", (tid));

    pthread_mutex_lock(&lock3);
    //while(doctors_waiting == 0 &&  num_of_doc_in_check ==0){
   //while(1){

        pthread_cond_signal(&notify11);
        pthread_cond_wait(&notify3 ,&lock3);
   // }
    count2++;

    if (count2 >= (persons1->num_of_doctor) ){
        count2 = 0;
    }

    //pthread_cond_broadcast(&notify3);
    pthread_mutex_unlock(&lock3);
    num_of_poeple_sofa --;
    num_of_poeple_waiting --;
    getMedicalCheckup(persons1);
}




pthread_mutex_t lock13;
pthread_cond_t notify13;

void ForPatients(void *arg){

     pthread_mutex_lock(&lock11);
    doctors_waiting ++;

    //pthread_mutex_lock(&lock11);
    struct person *persons1 = (struct person*)arg;
    printf("Medical Professional  %d ", (persons1->num));
    pid_t tid = gettid();
    printf("(Thread ID: %d", (tid));
    printf("): Waiting for patient  \n");


    //while( num_of_poeple_sofa == 0 ){
        pthread_cond_wait(&notify11 ,&lock11);
    //}


   // pthread_cond_broadcast(&notify11);

    pthread_mutex_unlock(&lock11);
    //pthread_cond_broadcast(&notify3);
    getMedicalCheckup(persons1);
    doctors_waiting --;

}

pthread_mutex_t lock5;
pthread_mutex_t lock4;
pthread_cond_t notify4;
int count3 = 0;
void getMedicalCheckup(void *arg){


    pthread_mutex_lock(&lock4);
    //pthread_cond_signal(&notify4);

    struct person *persons1 = (struct person*)arg;

    if(persons1->num < persons1->num_of_doctor){
        num_of_doc_in_check++;
        pthread_cond_signal(&notify3);
        pid_t tid = gettid();
        printf("Medical Professional  %d (Thread ID: %d): Checking Patient ", (persons1->num), tid);
    }
    else{
        printf(" %d \n", (persons1->num-persons1->num_of_doctor));
    }


    //pthread_cond_wait(&notify4 ,&lock4);

   // usleep(100);


    //pthread_cond_broadcast(&notify4);
    pthread_mutex_unlock(&lock4);
    pthread_mutex_lock(&lock5);
    perfromMedicalCheckup(persons1);
}


pthread_cond_t notify5;

pthread_mutex_t lock15;
pthread_cond_t notify15;

pthread_mutex_t lock16;
pthread_cond_t notify16;

pthread_mutex_t lock100;
pthread_cond_t notify100;


int count4 = 0;
//void makePayment(void *arg){
void perfromMedicalCheckup(void *arg){

    //pthread_cond_wait(&notify15 ,&lock15);

    struct person *persons1 = (struct person*)arg;

    if(persons1->num > persons1->num_of_doctor-1){
        //pthread_cond_signal(&notify16);
        pid_t tid = gettid();
        printf("Patient %d (Thread ID: %d ):  Getting checkup \n", (persons1->num-persons1->num_of_doctor), tid);
    }
    else{
       // pthread_cond_wait(&notify16 ,&lock5);
        //printf("here");
        //printf(" %d \n", (persons1->num));
    }

   // pthread_cond_wait(&notify5 ,&lock5);

    pthread_mutex_unlock(&lock5);
    pthread_mutex_lock(&lock100);
    //pthread_mutex_unlock(&lock5);
    makePayment(persons1);
}

pthread_mutex_t lock6;
pthread_cond_t notify6;
int count5 = 0;


/**med proff here*/




pthread_mutex_t lock8;
pthread_cond_t notify8;

pthread_mutex_t lock101;
pthread_cond_t notify101;

pthread_mutex_t lock102;
pthread_cond_t notify102;

pthread_cond_t notify103;
int count7 = 0;
//void perfromMedicalCheckup(void *arg){
void makePayment(void *arg){
    //pthread_cond_wait(&notify15 ,&lock15);
    //printf("hihiasdhiohdas\n");

    struct person *persons1 = (struct person*)arg;

    if(persons1->num > persons1->num_of_doctor-1){
        //pthread_mutex_lock(&lock100);
        // pthread_cond_wait(&notify103 ,&lock100);

        pid_t tid = gettid();
         //pthread_mutex_unlock(&lock100);

        printf("Patient %d (Thread ID: %d ): Making Payment to Medical Professional  ", (persons1->num-persons1->num_of_doctor), tid);
        pthread_cond_signal(&notify16);

    }
    else{
        pthread_cond_wait(&notify16 ,&lock100);
        //printf("here");
        printf(" %d \n", (persons1->num));
        pthread_cond_signal(&notify102);
    }
     //pthread_mutex_unlock(&lock100);

    //pthread_cond_wait(&notify5 ,&lock5);

    pthread_mutex_unlock(&lock100);
     pthread_mutex_lock(&lock101);
    //pthread_cond_broadcast(&notify5);
    //pthread_mutex_unlock(&lock5);
    acceptPayment(persons1);
}

pthread_cond_t notify104;
pthread_mutex_t lock9;
pthread_cond_t notify9;
pthread_mutex_t lock103;
int count8 = 0;
void acceptPayment(void *arg){
    //pthread_cond_wait(&notify15 ,&lock15);
    //printf("hihiasdhiohdas\n");
    struct person *persons1 = (struct person*)arg;

    if(persons1->num > persons1->num_of_doctor-1){

        pid_t tid = gettid();
         pthread_cond_wait(&notify102 ,&lock101);

        printf("Patient2 %d (Thread ID: %d ): apptect  Professional  ", (persons1->num-persons1->num_of_doctor), tid);
        pthread_cond_signal(&notify9);
    }
    else{
        pthread_cond_wait(&notify9 ,&lock101);
        //printf("here2");
        printf(" %d \n", (persons1->num));
        pthread_cond_signal(&notify104);
    }

   // pthread_cond_wait(&notify5 ,&lock5);

    pthread_mutex_unlock(&lock101);
    pthread_mutex_lock(&lock103);;
    //pthread_mutex_unlock(&lock5);
    leaveClinic(persons1);
}

void leaveClinic(void *arg){

    struct person *persons1 = (struct person*)arg;

    doneCOunt++;
    if(persons1->num > persons1->num_of_doctor-1){
        pthread_cond_wait(&notify104 ,&lock103);
        printf("Patient %d has left with check up\n", (persons1->num-persons1->num_of_doctor));
        }
    else{
        pthread_mutex_unlock(&lock103);
        ForPatients(persons1);
    }


    pthread_mutex_unlock(&lock103);

}

