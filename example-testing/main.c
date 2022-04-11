#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/time.h>
#include <unistd.h>

#include "threadpool.h"

sem_t sem;
sem_t sem2;
sem_t sem3;
sem_t sem4;

sem_t waiting;

int num_of_poeple_waiting = 0;
int num_of_poeple_sofa = 0;
int count = 0;
int check_up_time = 0;
int MainDone = 0;
int standing_line = 0;

struct timeval now, start, stop, enter;

int main(int argc, char **argv){

    /** semaphore init*/
    sem_init(&sem, 0, 1);
    sem_init(&sem2, 0, 2);

    sem_init(&sem3, 0, 1);
    sem_init(&sem4, 0, 1);

    sem_init(&waiting, 0, 1);

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
        inputs.wait_room_cap = 5;
        inputs.num_of_sofa = 3;
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
    Args: thread size, queue size(not used) */
    tpool_t *pool1;
    assert((pool1 = create_pool(inputs.num_of_pat+inputs.num_of_med_prof+2, inputs.num_of_pat = 50)) != NULL);

    /** add the doctors to the main functions
    args: pool struct, function, and the struct for the thread to be passed to keep track of vars*/
    for(int i = 0; i < inputs.num_of_med_prof; i++){
        pool_new_task(pool1, &ForPatients, &persons[i]);
        count++;
    }

    //struct timeval stop, start;




    /** while less then 100 pat have went through the hospital
    needs to be changed to input struct pat size */

    gettimeofday(&start, NULL);

    while(count <= inputs.num_of_pat + inputs.num_of_med_prof){

        /** delay counter for people coming and waiting at door */
        usleep(inputs.max_arr_time*1000);

        /** adds next person to waitingroom with their struct (struct tells them what num person they are)*/
        pool_new_task(pool1, &leaving_no_checkUp, &persons[count]);
        count++;
        }

    //printf("jkhdaskjhdkajshjkdsa %d\n", num_of_poeple_waiting);
    //printf("\n");


    while(num_of_poeple_sofa != 0 && count != inputs.num_of_pat + inputs.num_of_med_prof){
    }

    gettimeofday(&stop, NULL);

    MainDone = 1;
    usleep(150*1000);
    pthread_cond_broadcast(&notify3);
    usleep(inputs.max_arr_time*1000);
    printf("Done, now ending\n");

    printf("Total Time in Hospital %lu s\n", ((stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec));

    /** FREES pool*/
    free(pool1);

    return 0;
}

pid_t tid10;

void leaving_no_checkUp(void *arg){
    //struct person *persons1 = (struct person*)arg;
    //printf("jkhdaskjhdkajshjkdsa %d\n", persons1->num - inputs.num_of_med_prof);
    //printf("\n");


    /** gets thread id*/
    tid10 = gettid();

    /** sets up passed struct*/
    struct person *persons1 = (struct person*)arg;
    //sem_wait(&sem);
    gettimeofday(&now, NULL);
    printf("Patient %d \t\t(Thread ID: %d): Arriving the clinic \t\t\t %lu\n",persons1->num - inputs.num_of_med_prof, tid10,(now.tv_sec*1000000+now.tv_usec));
    //sem_post(&sem);

    /**maximum arrival interval*/
    usleep(inputs.max_arr_time*1000);

    //printf("jkhdaskjhdkajshjkdsa %d\n", num_of_poeple_waiting);
    //printf("\n");


    //printf("people wait: %d\n", num_of_poeple_waiting);
    if(num_of_poeple_waiting >= inputs.wait_room_cap){

        /** gets thread id*/
        tid10 = gettid();
        //sem_wait(&sem);
        gettimeofday(&now, NULL);
        printf("Patient %d \t\t(Thread ID: %d): Leaving the clinic without checkup \t %lu\n",persons1->num - inputs.num_of_med_prof, tid10,(now.tv_sec*1000000+now.tv_usec));
        //sem_post(&sem);
            while(1){
                //printf("\n");
            }
        }
    else{
        enterWaitingRoom(persons1);
    }
}
pid_t tid11;
void enterWaitingRoom(void *arg){

    sem_wait(&waiting);
    num_of_poeple_waiting ++;
    sem_post(&waiting);


    /** sets up passed struct*/
    struct person *persons1 = (struct person*)arg;

    /** prints the patients number ( minues the amount of doctors or else it wouldn't start at 0)*/
    tid11 = gettid();
    gettimeofday(&enter, NULL);
    long t;
    t = enter.tv_sec*1000000 + enter.tv_usec;
    printf("Patient %d \t\t(Thread ID: %d): Entering the clinic \t\t\t %lu\n", (persons1->num-persons1->num_of_doctor),tid11, t);

    /**locks function*/
    pthread_mutex_lock(&lock1);
    /** broadcast notify1 */
    pthread_cond_broadcast(&notify1);

    /** unlock function*/
    pthread_mutex_unlock(&lock1);
    while(num_of_poeple_waiting-1 > num_of_poeple_sofa && num_of_poeple_sofa != persons1->num_of_sofa){
    };

    /**while there is no room on sofa wait until notified*/
    int x = 0;
    int y = 1;
    while(num_of_poeple_sofa >= persons1->num_of_sofa || standing_line == y){
        if(x == 0){
            gettimeofday(&now, NULL);
            printf("Patient %d \t\t(Thread ID: %d): Standing in the waiting room \t\t %lu\n",(persons1->num-persons1->num_of_doctor),tid11,(now.tv_sec*1000000+now.tv_usec));
            x++;
            standing_line++;
            y = standing_line;
        }
        pthread_cond_wait(&notify1 ,&lock1);
    }

    

    /** go to next function*/
    sitOnSofa(persons1);

}
pid_t tid12;
void sitOnSofa(void *arg){

   /** updates number of people on sofa*/
    sem_wait(&waiting);
    num_of_poeple_sofa++;
    sem_post(&waiting);

    /** sets up passed struct*/
    struct person *persons1 = (struct person*)arg;

     /** prints the patients number, get thread id*/
    tid12 = gettid();
    gettimeofday(&now, NULL);
    printf("Patient %d \t\t(Thread ID: %d): Sitting on a sofa in the waiting room \t %lu\n", (persons1->num-persons1->num_of_doctor), tid12, (now.tv_sec*1000000+now.tv_usec));

    /** locks func*/
    //pthread_mutex_lock(&lock2);

    /** signal notify11 not broadcast  signals doc someone is ready*/
    pthread_cond_signal(&notify3);

    /** wait until notify2 goes off*/
    pthread_cond_wait(&notify2 ,&lock2);


    /** unlock func and update vars and leave */

    pthread_mutex_unlock(&lock2);
    sem_wait(&waiting);
    num_of_poeple_sofa --;
    num_of_poeple_waiting --;
    sem_post(&waiting);
    getMedicalCheckup(persons1);
}
pid_t tid13;
void ForPatients(void *arg){

    /** locks func*/
     pthread_mutex_lock(&lock3);

    /** makes passed struct printf needs to be one line*/
    struct person *persons1 = (struct person*)arg;
    tid13 = gettid();
    gettimeofday(&now, NULL);
    printf("Medical Professional %d \t(Thread ID: %d): Waiting for patient \t\t\t %lu\n", (persons1->num), tid13, (now.tv_sec*1000000+now.tv_usec));

    /** waits for pat*/
    if(num_of_poeple_sofa==0){

        pthread_cond_wait(&notify3 ,&lock3);

    }
    if(MainDone != 1){
        /** unlocks and updates vars*/
        pthread_mutex_unlock(&lock3);

        getMedicalCheckup(persons1);
    }
    pthread_mutex_unlock(&lock3);
}

int workAroundRealDocNumber3 = 0;
int workAroundRealpatNumber3 = 55;
//int tid2 = 0;
pid_t tid3;

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
        gettimeofday(&now, NULL);
        printf("Medical Professional %d \t(Thread ID: %d): Checking Patient %d \t\t\t %lu\n", (workAroundRealDocNumber3), tid3, workAroundRealpatNumber3, (now.tv_sec*1000000+now.tv_usec));
        sem_post(&sem);
    }

    /**unlocks and locks back to keep next function safe*/
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
        gettimeofday(&now, NULL);
        printf("Patient %d \t\t(Thread ID: %d): Getting checkup \t\t\t\t %lu\n", (persons1->num-persons1->num_of_doctor), tid14, (now.tv_sec*1000000+now.tv_usec));
    }
    else{

    }

    usleep(inputs.pat_check_time*1000);
    /**unlocks and locks back to keep next function safe*/
    makePayment(persons1);
}

int workAroundRealDocNumber2 = 0;
int workAroundRealpatNumber2 = 55;
//int tid2 = 0;
pid_t tid2;
int dochere = 0;

pthread_mutex_t lock7;
pthread_cond_t notify9;


pthread_mutex_t lock10;
pthread_cond_t notify10;

pthread_mutex_t lock11;
pthread_cond_t notify11;

int patsnumber = 5555;
int yourdocfound = 0;
int wait = 0;
int workAroundRealpatNumber2print = 0;

void makePayment(void *arg){

    struct person *persons1 = (struct person*)arg;

    /** if your a patienit enter if loop*/
    if(persons1->num > persons1->num_of_doctor-1){

        sem_wait(&sem3);

        workAroundRealpatNumber2 = persons1->seeing_doc_num;
        workAroundRealpatNumber2print = persons1->num-persons1->num_of_doctor;
        tid2 = gettid();

        wait = 1;
        pthread_cond_wait(&notify10 ,&lock10);
        pthread_mutex_unlock(&lock10);
        wait = 0;
        sem_post(&sem3);


    }
    /** if your a doctor enter here*/
    else{


        while(workAroundRealpatNumber2 != persons1->num){

        }
        while(wait ==0){

        }

        yourdocfound ==1;

        workAroundRealDocNumber2=persons1->num;
        gettimeofday(&now, NULL);
        printf("Patient %d \t\t(Thread ID: %d): Making Payment to Medical Professional %d \t %lu\n", (workAroundRealpatNumber2print), tid2, workAroundRealDocNumber2, (now.tv_sec*1000000+now.tv_usec));
        pthread_cond_signal(&notify10);
        pthread_cond_signal(&notify7);
         pthread_cond_signal(&notify9);

         yourdocfound =0 ;


    }

    /**unlocks and locks back to keep next function safe*/

     pthread_mutex_lock(&lock7);
    acceptPayment(persons1);
}


pthread_mutex_t lock7;
pthread_cond_t notify8;

int workAroundRealDocNumber = 0;
int workAroundRealpatNumber = 0;
int tid1 = 0;

void acceptPayment(void *arg){

    struct person *persons1 = (struct person*)arg;

    /** if your a patienit enter if loop*/
    if(persons1->num > persons1->num_of_doctor-1){


        //tid1 = tid;
         pthread_cond_wait(&notify8 ,&lock7);
        sem_wait(&sem);

        /** notify next print*/
        workAroundRealpatNumber = persons1->num-persons1->num_of_doctor;
        pthread_cond_signal(&notify5);
    }
    else{
        /** wait to print*/
        pthread_cond_signal(&notify8);
        pthread_cond_wait(&notify5 ,&lock7);

        workAroundRealDocNumber=persons1->num;
        tid1 = gettid();
        gettimeofday(&now, NULL);
        printf("Medical Professional %d \t(Thread ID: %d): Accepting Payment from Patient %d\t\t %lu\n", (workAroundRealDocNumber), tid1, workAroundRealpatNumber, (now.tv_sec*1000000+now.tv_usec));
        /** notify next print*/
        pthread_cond_signal(&notify6);
        sem_post(&sem);
    }

    pthread_mutex_unlock(&lock7);
    pthread_mutex_lock(&lock7);;
    leaveClinic(persons1);
}
pid_t tid15;
void leaveClinic(void *arg){

    struct person *persons1 = (struct person*)arg;

    /** if pat enter*/
    if(persons1->num > persons1->num_of_doctor-1){
        pthread_cond_wait(&notify6 ,&lock7);
        sem_wait(&sem);
        tid15 = gettid();
        gettimeofday(&now, NULL);
        printf("Patient %d \t\t(Thread ID: %d): Leaving the clinic after receiving checkup %lu\n", (persons1->num-persons1->num_of_doctor),tid15, (now.tv_sec*1000000+now.tv_usec));
        sem_post(&sem);
        }
    else{
        /** doctor go back to start*/
        pthread_mutex_unlock(&lock7);
        ForPatients(persons1);
    }

    /** unlock for pat to finish*/
    pthread_mutex_unlock(&lock7);
}
