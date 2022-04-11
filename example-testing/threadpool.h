#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include <semaphore.h>

typedef struct threadpool_t threadpool_t;


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
    struct timeval stop, start;
};

typedef struct {
    void (*function)(void *);
    struct person *argument;
} threadpool_task_t;

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

void WaitForPatients(void *arg);
void enterWaitingRoom(void *arg);
void sitOnSofa(void *arg);
void getMedicalCheckup(void *arg);
void makePayment(void *arg);
void leaveClinic(void *arg);
void perfromMedicalCheckup(void *arg);
void acceptPayment(void *arg);
void leaving_no_checkUp(void *arg);
void docleave(void *arg);
void *keep_thread(void *in_pool);
void pool_new_task(tpool_t *in_pool, void (*function) (void*), struct person *in_person); //add new task to thread pool
int end_pool(tpool_t *in_pool);
tpool_t *create_pool(int thread_count, int task_count);


struct input {
  int num_of_med_prof;
  int num_of_pat;
  int wait_room_cap;
  int num_of_sofa;
  int max_arr_time;
  int pat_check_time;
};

struct input inputs;

struct threadpool_t {
  pthread_mutex_t lock;
  pthread_cond_t notify;
  pthread_t *threads;
  threadpool_task_t *queue;
  int queue_size;
  int head;
  int tail;
  int count;
};

void* threadpool_thread(void *threadpool);
struct threadpool_t *threadpool_create(int thread_count, int queue_size);
int threadpool_add(threadpool_t *pool, void (*routine)(void *),struct person *persons1);
int threadpool_destroy(threadpool_t *pool);


#endif











