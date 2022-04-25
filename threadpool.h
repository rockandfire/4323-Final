#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/time.h>
#include <semaphore.h>
#include <time.h>

struct person{ //contains information relating to a patient/doctor, argument for keep_thread_alive
    int num;
    int num_of_doctor;
    int num_of_sofa;
    int paired_tid;
    int seeing_doc_num;
    struct timeval stop, start;
};

struct pat_queue //ensures fifo for standing/sofas
{
  struct timeval *times;
  int current;
  int end;
};

typedef struct //allows easy calls for different methods based on input type in keep_thread_alive
{
    void (*function) (void*);
    struct person *arg;
} tpool_task_t;

typedef struct //contains info associated with thread pool
{
  pthread_mutex_t task_mut;
  pthread_cond_t task_new;
  pthread_t *thread_arr;
  tpool_task_t *task_arr;
  int task_add_pos;
  int task_count;
  int current_task_pos;
} tpool_t;

struct input { //struct for user arguments implemented for readability
  int num_of_med_prof;
  int num_of_pat;
  int wait_room_cap;
  int num_of_sofa;
  int max_arr_time;
  int pat_check_time;
};

/** sharing all mutex/cond/sem to prevent scope issues */
extern pthread_mutex_t NEW_ENTRY; //all extern mutex to share between all files
extern pthread_mutex_t WAIT_FOR_DOC;
extern pthread_mutex_t WAIT_FOR_PATIENT;
extern pthread_mutex_t GET_CHECKUP;
extern pthread_mutex_t LEAVE_WAIT;
extern pthread_mutex_t DATA_ENTRY;
extern pthread_mutex_t END;
extern pthread_mutex_t LOCK_MAKE_PAYMENT;

extern pthread_cond_t SOFA_OPEN; //all extern conditions to share between all files
extern pthread_cond_t DOCTOR_AVAILABLE;
extern pthread_cond_t PATIENT_READY;
extern pthread_cond_t PAYMENT_COMPLETE;
extern pthread_cond_t NOTIF_LEAVE_WAIT;
extern pthread_cond_t NEW_TASK;
extern pthread_cond_t ALL_DONE;
extern pthread_cond_t PAYMENT_ACCEPTED;
extern pthread_cond_t MAKING_PAYMENT;

extern sem_t SEM_LEAVE_CHECKUP; //all extern semaphores to share between all files
extern sem_t SEM_MAKE_PAYMENT;
extern sem_t SEM_WAITING_ROOM;
extern sem_t SEM_PAYMENT_ACCEPT;
extern sem_t SEM_CHECKUP;

extern int MainDone; //all shared counters/structs
extern int num_of_people_waiting;
extern int num_of_people_sofa;
extern int counter;
extern int successfulCheckups;
extern int numOfPeopleThatLeft;
extern float averageWaitTimeForPat;
extern float averageWaitTimeForDoc;
extern int doctCounter;
extern int wait;
struct input inputs;
struct pat_queue standing_queue;

void enterWaitingRoom(void *arg); //all methods for project
void sitOnSofa(void *arg);
void getMedicalCheckup(void *arg);
void performMedicalCheckup(void *arg);
void makePayment(void *arg);
void acceptPayment(void *arg);
void leaveClinic(void *arg);
void waitForPatients(void *arg);
void leaveOrEnter(void *arg);
void *keep_thread_alive(void *in_pool);
tpool_t *create_pool(int thread_count, int task_count);

#endif











