#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include<stdlib.h>

pthread_mutex_t hospital=PTHREAD_MUTEX_INITIALIZER;




#define HOSPITAL_ROOM 10 
#define Patient 10

void *tretment(void *args){
    pthread_mutex_lock(&hospital);
    sleep(rand()%5);
    printf("Patient %d have been treated by the docotr %lu\n",*(int*)args,(unsigned long)pthread_self());
    pthread_mutex_unlock(&hospital);
    return NULL;
}

int main(void){
    pthread_t room[HOSPITAL_ROOM];
    for (int i=0;i<Patient;i++){
        int *a=(int*)malloc(sizeof(int));
        *a=i;
        pthread_create(&room[i],NULL,tretment,a);
    }
    for (int i=0;i<Patient;i++){
        pthread_join(room[i],NULL);
    }
}