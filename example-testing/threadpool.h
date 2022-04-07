#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_


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

struct person{
    int num;
    int done;
    int num_of_pat_left;
    int num_of_doctor;
    int num_of_sofa;
    int paired_tid;
    int pat_num;
    int doc_num;

};

typedef struct {
    void (*function)(void *);
    struct person *argument;
} threadpool_task_t;


void ForPatients(void *arg);
void enterWaitingRoom(void *arg);
void sitOnSofa(void *arg);
void getMedicalCheckup(void *arg);
void makePayment(void *arg);
void leaveClinic(void *arg);
void perfromMedicalCheckup(void *arg);
void acceptPayment(void *arg);
void leaving_no_checkUp(void *arg);
void docleave(void *arg);

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

void threadpool_thread(void *threadpool);
struct threadpool_t *threadpool_create(int thread_count, int queue_size);
int threadpool_add(threadpool_t *pool, void (*routine)(void *),struct person *persons1);
int threadpool_destroy(threadpool_t *pool);


#endif











