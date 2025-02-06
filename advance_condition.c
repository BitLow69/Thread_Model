/*📝 Task: Implement Timed Waiting
💡 Task: Modify the above producer-consumer problem so that if a consumer waits for more than 3 seconds, it stops waiting and prints:
"Consumer timed out while waiting for data."

📌 Requirements:
🔹 Use pthread_cond_timedwait() instead of pthread_cond_wait().
🔹 If no data is produced within 3 seconds, the consumer should exit instead of waiting indefinitely.

Let me know once you’ve attempted the task! 🚀*/

#include <errno.h>
#include<stdio.h>
#include<unistd.h>
#include<time.h>
#include<pthread.h>
#include<stdlib.h>

#define PRODUCER 10
#define CONSUMER 10
#define BUFFER 5 

int buffer [BUFFER]; //this is to maintain that at the momeny only 5 items will be at the markets 
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t producer_cond=PTHREAD_COND_INITIALIZER;
pthread_cond_t consumer_cond=PTHREAD_COND_INITIALIZER;
int producer_flag=0;  // this will set to 1 once producer produce item 

int count=0;
void* producer(void* args){
    pthread_mutex_lock(&mutex);
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME,&ts);
    ts.tv_sec+=5;
    int *producer_id =(int*)args;
    while(count==5){
        int res=pthread_cond_timedwait(&producer_cond,&mutex,&ts);
        if (res==ETIMEDOUT){
            printf("No conumser were found so producer %d exit withoug producing as buffer is full. \n",*producer_id);
            pthread_mutex_unlock(&mutex);
            return NULL;
        }
    }
    sleep(5);
    buffer[count++]=*producer_id;
    producer_flag=1;
    pthread_cond_broadcast(&consumer_cond);   
    printf("Producer %d: producer the item \n",buffer[count-1]);
    free(producer_id);
    pthread_mutex_unlock(&mutex);
   
    return NULL;

}
void * consumer(void* args){
    pthread_mutex_lock(&mutex);
    int *consumer_id=(int *)args;
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME,&ts);
    ts.tv_sec+=3;
    while(!producer_flag){
        printf("consumer %d  is waiting\n",*consumer_id);
        int res=pthread_cond_timedwait(&consumer_cond,&mutex,&ts);
        if (res==ETIMEDOUT){
            printf("Worker %d have waited more than 3 seconds exiting now\n",*consumer_id);
            pthread_mutex_unlock(&mutex);
            return NULL;
        }

    }
    printf("Consumer %d: have consumed the product producer by %d\n",*consumer_id,buffer[--count]);
    pthread_cond_broadcast(&producer_cond);
    if (count==0){
        producer_flag=0;
    }
    free(consumer_id);
    pthread_mutex_unlock(&mutex);
    pthread_cond_broadcast(&producer_cond);
    return NULL;

}
int main(){
    pthread_t producers[PRODUCER],consumers[CONSUMER];
     for (int i=0;i<CONSUMER;i++){
        int* a=(int *)malloc(sizeof(int));
        *a=i+1;
        if((pthread_create(&consumers[i],NULL,consumer,a))!=0){
            perror("Error in creating the thread");
        }
    }
    for (int i=0;i<PRODUCER;i++){
        int* a=(int *)malloc(sizeof(int));
        *a=i+1;
        if((pthread_create(&producers[i],NULL,producer,a))!=0){
            perror("Error in creating the thread");
        }
    }
   
    for (int i=0;i<PRODUCER;i++){
        if ((pthread_join(producers[i],NULL))!=0){
            perror("Error in joining the thread\n");

        }
    }
    for (int i=0;i<CONSUMER;i++){
        if ((pthread_join(consumers[i],NULL))!=0){
            perror("Error in joining the thread\n");
            
        }
    }


}