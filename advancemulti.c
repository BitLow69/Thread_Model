/*
🛠 Task: Implement Multi-Condition Synchronization
🚀 Task: Implement a Car Wash System 🔹 You have 3 types of workers:

Washers: Only work when dirty cars arrive.
Dryers: Only work when washed cars are available.
Polishers: Only work when dried cars are available.
🔹 Use 3 separate condition variables:

pthread_cond_t washer_cond;
pthread_cond_t dryer_cond;
pthread_cond_t polisher_cond;
🔹 Each worker type should only wake up when needed to prevent unnecessary wake-ups.

*/

#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h>
#include<errno.h>


#define QUEUE_SIZE 6
#define WORKERS 4
#define DRYERS 3
#define POLISHERS 2


int queue_size=0;
struct cars{
    int car_numbers;
};
void enqueue(pthread_mutex_t *lock,pthread_cond_t *en,pthread_cond_t *de,struct cars *queue_array,struct cars data,int *count,int *rear,int *front){
    // printf("here \n");
    pthread_mutex_lock(lock);
    while(*count==QUEUE_SIZE){
        // printf("sttuck here");
        // printf("reached here \n");
        pthread_cond_wait(en,lock);
    }
    queue_array[*rear]=data;
    (*count)++;
    *rear=((*rear+1)%QUEUE_SIZE);
  
    pthread_mutex_unlock(lock);
    // printf("have reached hearer\n");
      pthread_cond_signal(de);

}

struct cars dequeu(pthread_mutex_t *lock,pthread_cond_t *en,pthread_cond_t *de,struct cars *queue_array,int* count,int* rear,int* front){
    // printf("here \n");
    pthread_mutex_lock(lock);
    while(*count==0){
        // printf("reached here  dedu\n");
        pthread_cond_wait(de,lock);
        
        }

    struct cars a=queue_array[*front];
    (*count)-- ;
    *front=(*front+1)%QUEUE_SIZE;
    pthread_mutex_unlock(lock);
    pthread_cond_signal(en);
    
    return a;


}





//WASEHD CAR QUEUUE
pthread_cond_t car_wash_queue_enqueue_cond=PTHREAD_COND_INITIALIZER;
pthread_cond_t car_wash_queue_dequeue_cond=PTHREAD_COND_INITIALIZER;
pthread_mutex_t car_wash_queue=PTHREAD_MUTEX_INITIALIZER;
int car_wash_count=0,car_wash_rear=0,car_wash_front=0;
struct cars car_wash[QUEUE_SIZE];




// DRIED CAR QUEUE
pthread_cond_t car_dry_queue_enqueue_cond=PTHREAD_COND_INITIALIZER;
pthread_cond_t car_dry_queue_dequeue_cond=PTHREAD_COND_INITIALIZER;
pthread_mutex_t car_dry_queue=PTHREAD_MUTEX_INITIALIZER;
int car_dry_count=0,car_dry_rear=0,car_dry_front=0;
struct cars dry_wash[QUEUE_SIZE];


int dirty_car=10;// this is for our we declared the number of dirty car we can use the thread pool instead of thread per task 
int tobepolishedcar=10;
int tobedryiedcar=10;

// seperate mutex for washer,dryer and polisehr as they work on various car i donnot want one to stop while other work so resources are differnet 

pthread_mutex_t washer_mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t dryer_mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t polisher_mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t dryer_cond=PTHREAD_COND_INITIALIZER;
pthread_cond_t polisher_cond=PTHREAD_COND_INITIALIZER;

//first worker they work when the car arrive at the station 
void* washers_func(void* args){
    
    while(dirty_car>0){
        // printf("here \n");
        pthread_mutex_lock(&washer_mutex);
        int car_number=rand()%10000+1000;//number have been provided while car is runnig 
        struct cars car;
        car.car_numbers=car_number;


        enqueue(&car_wash_queue,&car_wash_queue_enqueue_cond,&car_wash_queue_dequeue_cond,car_wash,car,&car_wash_count,&car_wash_rear,&car_wash_front);
                printf("%d car have been washed \n",car.car_numbers);

        dirty_car--;
        pthread_cond_signal(&dryer_cond);
        pthread_mutex_unlock(&washer_mutex);

    }
    return NULL;
}
// they wwork only when there are car that are already washed
void* dryers_func(void* args){

    while(tobedryiedcar>0){
        //  printf("here  driyer \n");
        pthread_mutex_lock(&dryer_mutex);
        while(car_wash_count==0){
            pthread_cond_wait(&dryer_cond,&dryer_mutex);

        }

    struct cars car=dequeu(&car_wash_queue,&car_wash_queue_enqueue_cond,&car_wash_queue_dequeue_cond,car_wash,&car_wash_count,&car_wash_rear,&car_wash_front);
    printf("%d car have been dried\n",car.car_numbers);
    enqueue(&car_dry_queue,&car_dry_queue_enqueue_cond,&car_dry_queue_dequeue_cond,dry_wash,car,&car_dry_count,&car_dry_rear,&car_dry_front);
    pthread_cond_signal(&polisher_cond);
    tobedryiedcar--;
    pthread_mutex_unlock(&dryer_mutex);

    }
    
    
    return NULL;
}
// they work only  when the car have been washed adn dried
void* polisher_func(void* args){
    while(tobepolishedcar>0){
         pthread_mutex_lock(&polisher_mutex);
        while(car_dry_count==0){
            pthread_cond_wait(&polisher_cond,&polisher_mutex);

        }
        struct cars car=dequeu(&car_dry_queue,&car_dry_queue_enqueue_cond,&car_dry_queue_dequeue_cond,dry_wash,&car_dry_count,&car_dry_rear,&car_dry_front);
        printf("%d car have been polished \n",car.car_numbers);
        // printf("%d\n",car_dry_count);
        tobepolishedcar--;
        pthread_mutex_unlock(&polisher_mutex);
    

    }
    return NULL;
   
}

int main(){
    pthread_t washer[WORKERS],polisher[POLISHERS],dryer[DRYERS]; // CREATE A SEPERATE THEREAD FOR EACH DIRTY CAR THIS MEANS WORKERS FOR DIRTY CAR OR I CAN JUST MAKE A POOL OF WORKER 
    for (int i=0;i<WORKERS;i++){
        if ((pthread_create(&washer[i],NULL,washers_func,NULL))!=0){
            perror("Error in creating the thread\n");
        }
    }
    for (int i=0;i<POLISHERS;i++){
        if ((pthread_create(&polisher[i],NULL,polisher_func,NULL))!=0){
            perror("Error in creating the thread\n");
        }
    }for (int i=0;i<DRYERS;i++){
        if ((pthread_create(&dryer[i],NULL,dryers_func,NULL))!=0){
            perror("Error in creating the thread\n");
        }
    }
    for (int i=0;i<WORKERS;i++){
        pthread_join(washer[i],NULL);
    }
    for (int i=0;i<POLISHERS;i++){
        pthread_join(polisher[i],NULL);
    }
    for (int i=0;i<DRYERS;i++){
        pthread_join(dryer[i],NULL);
    }


}


