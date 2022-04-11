#include "threadpool.h"


int num_of_poeple_waiting = 0;
int num_of_poeple_sofa = 0;
int counter = 0;
int check_up_time = 0;

int successfulCheckups = 0;
int numOfPeopleThatLeft = 0;
float averageWaitTimeForPat = 0;
float averageWaitTimeForDoc = 0;
int doctCounter = 0;
int standing_line = 0;
int stand = 0;

pthread_mutex_t lock;
pthread_mutex_t lock1;
pthread_mutex_t lock2;
pthread_mutex_t lock3;
pthread_mutex_t lock4;
pthread_mutex_t lock5;
pthread_mutex_t lock6;
pthread_mutex_t lock7;

pthread_cond_t notify1;
pthread_cond_t notify2;
pthread_cond_t notify3;
pthread_cond_t notify4;
pthread_cond_t notify5;
pthread_cond_t notify6;
pthread_cond_t notify7;

sem_t sem;
sem_t sem2;
sem_t sem3;
sem_t sem4;
sem_t sem5;
sem_t waiting;

int MainDone = 0;

int main(int argc, char **argv){

    /** semaphore init*/
    sem_init(&sem, 0, 1);
    sem_init(&sem2, 0, 2);
    sem_init(&sem3, 0, 1);
    sem_init(&sem4, 0, 1);
    sem_init(&waiting, 0, 1);
    sem_init(&sem5, 0, 2);

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
        inputs.num_of_med_prof = 4;
        inputs.num_of_pat = 50;
        inputs.wait_room_cap = 5;
        inputs.num_of_sofa = 3;
        inputs.max_arr_time = 10;
        inputs.pat_check_time = 100;
    }
        inputs.num_of_pat = 50;
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
    assert((pool1 = create_pool(inputs.num_of_pat+inputs.num_of_med_prof+2, inputs.num_of_pat)) != NULL);

    /** add the doctors to the main functions
    args: pool struct, function, and the struct for the thread to be passed to keep track of vars*/
    for(int i = 0; i < inputs.num_of_med_prof; i++){
        pool_new_task(pool1, &WaitForPatients, &persons[i]);
        counter++;
    }

    struct timeval stop, start;

    /** while less then 100 pat have went through the hospital
    needs to be changed to input struct pat size */

    gettimeofday(&start, NULL);

    while(counter <= inputs.num_of_pat + inputs.num_of_med_prof){

        /** delay counter for people coming and waiting at door */
        usleep(inputs.max_arr_time*1000);

        /** adds counter1 person to waitingroom with their struct (struct tells them what num person they are)*/
        pool_new_task(pool1, &leaving_no_checkUp, &persons[counter]);
        counter++;
    }


    while(num_of_poeple_sofa != 0 && counter != inputs.num_of_pat + inputs.num_of_med_prof){
    }

    gettimeofday(&stop, NULL);

    MainDone = 1;
    usleep(150*1000);
    pthread_cond_broadcast(&notify3);
    usleep(inputs.max_arr_time*1000);
    printf("Done, now ending\n");


    printf("\n");
    printf("\n");
    printf("\n");
    printf("Statistical Summary: \n");
    printf("=======================================================\n");
    printf("\n");
    printf("Number of successful checkups      : %d\n", successfulCheckups);
    printf("Number of Patients that left       : %d\n", numOfPeopleThatLeft);
    printf("Average wait time for patients     : %fms\n", (averageWaitTimeForPat/successfulCheckups)/1000);
    printf("Average wait for Medical Profess   : %fms\n", (averageWaitTimeForDoc/doctCounter)/1000);
    printf("\n");
    printf("=======================================================\n");


    /** FREES pool*/
    free(pool1);

    return 0;
}




//int tid2 = 0;



//int tid2 = 0;

int dochere = 0;

pthread_mutex_t lock7;





pthread_mutex_t lock11;
pthread_cond_t notify11;

int patsnumber = 5555;



