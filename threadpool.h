#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/time.h>
#include <semaphore.h>

struct person{
    int num;
    int done;
    int num_of_pat_left;
    int num_of_doctor;
    int num_of_sofa;
    int paired_tid;
    int pat_num;
    int seeing_doc_num;
    int made_it_through;
    int p_type;
    struct timeval stop, start;
};

struct pat_queue
{
  struct timeval *times;
  int current;
  int end;
};

typedef struct
{
    void (*function) (void*);
    struct person *arg;
} tpool_task_t;

typedef struct
{
  pthread_mutex_t lock;
  pthread_cond_t notify;
  pthread_mutex_t task_mut;
  pthread_cond_t new_task;
  pthread_t *thread_arr;
  tpool_task_t *task_arr;
  int task_add_pos;
  int task_count;
  int current_task_pos;
} tpool_t;

typedef struct {
    void (*function)(void *);
    struct person *argument;
} threadpool_task_t;

typedef struct
{
  tpool_t *in_pool;
  struct person *thread_person;
} thread_arg;

struct input {
  int num_of_med_prof;
  int num_of_pat;
  int wait_room_cap;
  int num_of_sofa;
  int max_arr_time;
  int pat_check_time;
};

extern pthread_mutex_t NEW_ENTRY;
extern pthread_mutex_t WAIT_FOR_DOC;
extern pthread_mutex_t WAIT_FOR_PATIENT;
extern pthread_mutex_t GET_CHECKUP;
extern pthread_mutex_t LEAVE_WAIT;
extern pthread_mutex_t DATA_ENTRY;
extern pthread_mutex_t END;
extern pthread_mutex_t LOCK_MAKE_PAYMENT;

extern pthread_cond_t SOFA_OPEN;
extern pthread_cond_t DOCTOR_AVAILABLE;
extern pthread_cond_t PATIENT_READY;
extern pthread_cond_t PAYMENT_COMPLETE;
extern pthread_cond_t NOTIF_LEAVE_WAIT;
extern pthread_cond_t NEW_TASK;
extern pthread_cond_t ALL_DONE;
extern pthread_cond_t PAYMENT_ACCEPTED;
extern pthread_cond_t MAKING_PAYMENT;

extern sem_t SEM_LEAVE_CHECKUP;
extern sem_t SEM_MAKE_PAYMENT;
extern sem_t SEM_WAITING_ROOM;
extern sem_t SEM_PAYMENT_ACCEPT;
extern sem_t SEM_CHECKUP;

extern int MainDone;
extern int num_of_people_waiting;
extern int num_of_people_sofa;
extern int counter;
extern int check_up_time;
extern int current_task_pos;

extern int successfulCheckups;
extern int numOfPeopleThatLeft;
extern float averageWaitTimeForPat;
extern float averageWaitTimeForDoc;
extern int doctCounter;
extern int wait;
extern int standing_line;
extern int stand;
struct input inputs;
struct pat_queue standing_queue;

void waitForPatients(void *arg);
void enterWaitingRoom(void *arg);
void sitOnSofa(void *arg);
void getMedicalCheckup(void *arg);
void makePayment(void *arg);
void leaveClinic(void *arg);
void performMedicalCheckup(void *arg);
void acceptPayment(void *arg);
void leaveOrEnter(void *arg);
void docleave(void *arg);
void *keep_thread_alive(void *in_pool);
void pool_new_task(tpool_t *in_pool, void (*function) (void*), struct person *in_person); //add new task to thread pool
int end_pool(tpool_t *in_pool);
tpool_t *create_pool(int thread_count, int task_count);

#endif











