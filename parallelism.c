#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#include<stdlib.h>
#define THREAD_SIZE 3
#define SIZE 10 


int data[SIZE]={1,2,3,4,5,6,7,8,9,10};
int factorial_value[SIZE];

int factorial_calculator(int n){
    if (n<=1){
        return 1;
    }else{
        return (n*factorial_calculator(n-1));
    }
}
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;

void *factorial(void *args){
    int *d=(int *)args;
    for (int i=*d;i<SIZE;i+=THREAD_SIZE){
        pthread_mutex_lock(&mutex);
        factorial_value[i]=factorial_calculator(data[i]);
        pthread_mutex_unlock(&mutex);

    }
    return NULL;
}
int main(){
    pthread_t th[THREAD_SIZE];
    for (int i=0;i<THREAD_SIZE;i++){
        int *a=(int*)malloc(sizeof(int));
        *a=i;
        pthread_create(&th[i],NULL,factorial,a);

    }
    for (int i=0;i<THREAD_SIZE;i++){
        pthread_join(th[i],NULL);
    }
    for (int i=0;i<SIZE;i++){
        printf("%d \t",factorial_value[i]);
    }
}