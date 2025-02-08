#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<stdatomic.h>
#include<time.h>
#include<unistd.h>


/*

first we will create the data structure for our char 
it will just shold two ifnfomrisotn that we need to pass to our thread 
one being the car number 
another its direction --- we will just store the direction for now we will only allow to go in straing they cannot go right,left or anytign north can go south viceveras
*/


/* 
General idea one universal thread that will be naemd intersection or traffic light it will changes it state 1 or 0 every 3 second it menas in those second we will move the 
car from that 
another car -- we will use thread per task as we nnednew thread for task , other than we could also create new thread which randomly chose between char nubmer and direction 
store in queue and we can just use 4 carthread to make them move thorgut the inteseciton using threadpool 



*/


struct Car{
    int car_number;
    char direction;
};
pthread_mutex_t intersection_mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t intersection_cond=PTHREAD_COND_INITIALIZER;
int flag=0;
#define CAR 20
int intersection_count=0;

void* intersection(void* args){
    while(intersection_count<CAR){
        pthread_mutex_lock(&intersection_mutex);
        flag =flag?0:1;
        printf("%d\n",intersection_count);
      
        pthread_cond_broadcast(&intersection_cond);
        pthread_mutex_unlock(&intersection_mutex);
    
        sleep(3);
    }
    return NULL;
}

pthread_mutex_t car_mutex=PTHREAD_MUTEX_INITIALIZER;
void *cars(void* args){
    struct Car car_info=*(struct Car*)args;
    if (car_info.direction=='E' || car_info.direction=='W'){
        pthread_mutex_lock(&car_mutex);
        while(!flag){  
            pthread_cond_wait(&intersection_cond,&car_mutex);
        }
        intersection_count++;
        printf("car with number %d  have travelled in %c \n",car_info.car_number,car_info.direction);
        pthread_mutex_unlock(&car_mutex);
        return NULL;
    }
    if (car_info.direction=='N' || car_info.direction=='S'){
        pthread_mutex_lock(&car_mutex);
        while(flag){  
            pthread_cond_wait(&intersection_cond,&car_mutex);
        }
        intersection_count++;
        printf("car with number %d  have travelled in %c \n",car_info.car_number,car_info.direction);
        pthread_mutex_unlock(&car_mutex);
    }
    return NULL;

}

const char direction[4]={'E','W','N','S'};


int main(){
    srand(time(NULL));
    pthread_t traffic;
    pthread_create(&traffic,NULL,intersection,NULL);
    pthread_t car[CAR];
    for (int i=0;i<CAR;i++){
        struct Car *a=(struct Car*)malloc(sizeof(struct Car ));
        a->car_number=rand()%1000+1000;
        a->direction=direction[rand()%4];
        pthread_create(&car[i],NULL,cars,a);

    }
    pthread_join(traffic,NULL);
    for (int i=0;i<CAR;i++){
        pthread_join(car[i],NULL);
    }
}