#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#include<stdlib.h>


#define SIZE 10


int prime_count=0;
int element_count=0;
int array[SIZE];
int prime_array[SIZE];
int array_sum=0;

pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond=PTHREAD_COND_INITIALIZER;

void *ran(void *args){
    for(int i=0;i<SIZE;i++){
        pthread_mutex_lock(&mutex);
        array[i]=rand()%100+1;
        element_count++;
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
    
}
int prime(int i){
    srand(time(NULL));
    if (i==1 || i<1){
        return 0;
    }
    if (i==2 || i==3 ){
        return 1;
    }
    for (int a=2;a<=i/2;a++){
        if (i%a==0){
            return 0;
        }

    }
 


    return 1;

}

void *isprime(void *args){
    for (int i=0;i<SIZE;i++){
        pthread_mutex_lock(&mutex);
        while(i>element_count){
            pthread_cond_wait(&cond,&mutex);
        }
        if (prime(array[i])){
            prime_array[prime_count]=array[i];
            prime_count++;

        }
        
       pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

void *sum(void *args){
    for (int i=0;i<SIZE;i++){
        pthread_mutex_lock(&mutex);
        while(i>element_count){
            pthread_cond_wait(&cond,&mutex);
        }
        array_sum+=array[i];
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(void){
    pthread_t th[3];
    pthread_create(&th[0],NULL,ran,NULL);
    pthread_create(&th[0],NULL,isprime,NULL);
    pthread_create(&th[0],NULL,sum,NULL);
    printf("Random Array : ");
    for (int i=0;i<SIZE;i++){
        printf(" %d \t ",array[i]);
    }
    printf("\n");
    printf("prime in that array \t ");
    for (int i=0;i<prime_count;i++){
        printf("%d \t",prime_array[i]);

    }
    printf("\n the sum of the random array is %d \n",array_sum);

}