//josh stuff

#include "threadpool.h"

pid_t tid11;
void enterWaitingRoom(void *arg){

    sem_wait(&waiting);
    num_of_poeple_waiting ++;
    sem_post(&waiting);


    /** sets up passed struct*/
    struct person *persons1 = (struct person*)arg;

    //gettimeofday(&stop, NULL);

    gettimeofday(&(persons1->start), NULL);

    //gettimeofday(&start, NULL);

    /** prints the patients number ( minues the amount of doctors or else it wouldn't start at 0)*/
    tid11 = gettid();
    printf("Patient %d (Thread ID: %d): Entering the clinic\n", (persons1->num-persons1->num_of_doctor),tid11);

    /**locks function*/
    pthread_mutex_lock(&lock1);

    /** broadcast notify1 */
    pthread_cond_broadcast(&notify1);

    /** unlock function*/
    pthread_mutex_unlock(&lock1);
    
    /** while loop to keep people who just entered into the room from cutting in line**/
    while(num_of_poeple_waiting-1 > num_of_poeple_sofa && num_of_poeple_sofa != persons1->num_of_sofa){
    };

    /**while there is no room on sofa wait until notified*/
    int x = 0;
    int y = 1;
    while(num_of_poeple_sofa >= persons1->num_of_sofa || y > 1+stand){
        if(x == 0){
            printf("Patient %d (Thread ID: %d): Standing in the waiting room\n",(persons1->num-persons1->num_of_doctor),tid11);
            x++;
            standing_line = num_of_poeple_waiting - num_of_poeple_sofa;
            y = standing_line+stand;
        }
        pthread_cond_wait(&notify1 ,&lock1);
    }
    stand++;

    /** go to counter1 function*/
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
    printf("Patient %d (Thread ID: %d): Sitting on a sofa in the waiting room \n", (persons1->num-persons1->num_of_doctor), tid12 );

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
    gettimeofday(&(persons1->stop), NULL);
    getMedicalCheckup(persons1);
}