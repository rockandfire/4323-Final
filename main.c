/**
 * @file main.c
 * @author Samuel Fipps (sfipps@okstate.edu)
 * @brief Group C
 * @date 04/25/22
 */

#include "threadpool.h"


int num_of_people_waiting = 0; //init counters/statistics
int num_of_people_sofa = 0;
int counter = 0;
int successfulCheckups = 0;
int numOfPeopleThatLeft = 0;
float averageWaitTimeForPat = 0;
float averageWaitTimeForDoc = 0;
int doctCounter = 0;

pthread_mutex_t WAIT_FOR_DOC; //init all mutexes
pthread_mutex_t WAIT_FOR_PATIENT;
pthread_mutex_t GET_CHECKUP;
pthread_mutex_t LEAVE_WAIT;
pthread_mutex_t NEW_ENTRY;
pthread_mutex_t DATA_ENTRY;
pthread_mutex_t END;
pthread_mutex_t LOCK_MAKE_PAYMENT;

pthread_cond_t SOFA_OPEN; //init all conditions
pthread_cond_t DOCTOR_AVAILABLE;
pthread_cond_t PATIENT_READY;
pthread_cond_t PAYMENT_COMPLETE;
pthread_cond_t NOTIF_LEAVE_WAIT;
pthread_cond_t NEW_TASK;
pthread_cond_t ALL_DONE;
pthread_cond_t PAYMENT_ACCEPTED;
pthread_cond_t MAKING_PAYMENT;

sem_t SEM_LEAVE_CHECKUP; //init all semaphores
sem_t SEM_MAKE_PAYMENT;
sem_t SEM_WAITING_ROOM;
sem_t SEM_PAYMENT_ACCEPT;
sem_t SEM_CHECKUP;

int MainDone = 0;
int random_time = 0;

int main(int argc, char **argv){ //main method

    /** semaphore init*/
    sem_init(&SEM_LEAVE_CHECKUP, 0, 1); //finish init semaphores
    sem_init(&SEM_MAKE_PAYMENT, 0, 1);
    sem_init(&SEM_WAITING_ROOM, 0, 1);
    sem_init(&SEM_PAYMENT_ACCEPT, 0, 1);
    sem_init(&SEM_CHECKUP, 0, 1);

    srand(time(NULL));

    /** struct to handle inputs*/
    if (argc == 7){
        inputs.num_of_med_prof  = atoi(argv[1]);
        inputs.num_of_pat       = atoi(argv[2]);
        inputs.wait_room_cap    = atoi(argv[3]);
        inputs.num_of_sofa      = atoi(argv[4]);
        inputs.max_arr_time     = atoi(argv[5]);
        inputs.pat_check_time   = atoi(argv[6]);
    }
    else{
        printf("Not enough args, exiting...\n");
        exit(0);
        /*inputs.num_of_med_prof = 4;
        inputs.num_of_pat = 50;
        inputs.wait_room_cap = 4;
        inputs.num_of_sofa = 1;
        inputs.max_arr_time = 10;
        inputs.pat_check_time = 100;*/
    }
        //inputs.num_of_pat = 40;
    /** struct to be passed around with threads*/
    struct person persons[inputs.num_of_med_prof + inputs.num_of_pat];

    /** initializes struct*/
    for (int i =0; i<=inputs.num_of_med_prof + inputs.num_of_pat; i++){
        persons[i].num = i;
        persons[i].num_of_doctor = inputs.num_of_med_prof;
        persons[i].num_of_sofa = inputs.num_of_sofa;
    }

    /** mutex init*/

    standing_queue.times = malloc(inputs.num_of_pat * sizeof(int));
    standing_queue.current = 0;
    standing_queue.end = 0;

    /** thread pool struct creation
    Args: thread size, queue size(not used) */
    tpool_t *pool1;
    pool1 = malloc(sizeof(tpool_t));
    pool1->thread_arr = malloc(inputs.num_of_med_prof + inputs.num_of_pat + 2 * sizeof(pthread_t));
    pool1->task_arr = malloc(inputs.num_of_pat * sizeof(tpool_task_t));
    pool1->task_count = 0;
    pool1->task_add_pos = 0;
    pool1->current_task_pos = 0;
    assert((pool1 = create_pool(inputs.num_of_med_prof + 15, inputs.num_of_pat)) != NULL);

    /** add the doctors to the main functions
    args: pool struct, function, and the struct for the thread to be passed to keep track of vars*/
    for(int i = 0; i < inputs.num_of_med_prof; i++){
        pool1->task_arr[pool1->task_add_pos].function = waitForPatients; //add new task to array
        pool1->task_arr[pool1->task_add_pos].arg = &persons[i];
        pool1->task_add_pos++; //update task array index and task count
        pool1->task_count++;
        counter++;
        pthread_cond_signal(&NEW_TASK);
    }

    struct timeval stop, start;

    /** while less then 100 pat have went through the hospital
    needs to be changed to input struct pat size */

    gettimeofday(&start, NULL);

    while(counter < inputs.num_of_pat + inputs.num_of_med_prof){ //begin allowing patients to arrive

        /** delay counter for people coming and waiting at door */
        random_time = rand() % inputs.max_arr_time;

        if (random_time == 0) //prevents a deadlock caused by weird arrival times
        {
            random_time = 1;
        }

        usleep(random_time * 1000); //arrive at a random time

        /** adds counter1 person to waitingroom with their struct (struct tells them what num person they are)*/
        pool1->task_arr[pool1->task_add_pos].function = leaveOrEnter; //add new task to array
        pool1->task_arr[pool1->task_add_pos].arg = &persons[counter];
        pool1->task_add_pos++; //update task array index and task count
        pool1->task_count++; //keep track of patients left for keep_thread_alive
        counter++;
        pthread_cond_signal(&NEW_TASK); //signal threads waiting in keep_thread_alive
    }
    
    while (inputs.num_of_pat > numOfPeopleThatLeft + successfulCheckups) //make sure all patients leave before ending program, leaveWait allows threads in this loop to check for end condition
    {
        pthread_cond_wait(&ALL_DONE, &END);
    }
    
    gettimeofday(&stop, NULL);

    MainDone = 1;
    //usleep(150*1000);
    //usleep(inputs.max_arr_time*1000);
    printf("Done, now ending\n"); //start displaying final statistics


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