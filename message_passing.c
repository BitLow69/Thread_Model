/*
Task 2: Message Passing - File Download Manager
Simulate a file download manager using producer-consumer threads.
A producer thread generates 10 file chunks, each with:
A unique chunk ID (1 to 10). 
A randomly generated size (100 KB to 1 MB).
2 consumer threads process these file chunks.
Use a queue to store a maximum of 3 file chunks at a time.
Log actions:
Producer logs each chunk it creates.
Consumers log each chunk they process.*/



#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>


#define CONSUMER_THREAD 2

#define queue_size 3
int start=0,count=0,rear=0;

struct file_info{
    int file_id;
    int size;
};

struct file_info a[queue_size];
pthread_mutex_t queue_mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_enqueu_cond=PTHREAD_COND_INITIALIZER;
pthread_cond_t queue_dequeu_cond=PTHREAD_COND_INITIALIZER;
int terminate=0;//flag
void enqueue(int file_id,int size){
    struct file_info file={file_id,size};
    pthread_mutex_lock(&queue_mutex);
    while(count==queue_size){
        pthread_cond_wait(&queue_enqueu_cond,&queue_mutex);
    }
    a[rear]=file;
    pthread_cond_broadcast(&queue_dequeu_cond);
    rear=(rear+1)%queue_size;
    count++;
    pthread_mutex_unlock(&queue_mutex);
}

struct file_info dequeue(){
    struct file_info file;
    pthread_mutex_lock(&queue_mutex);
    while (count==0){
        if(!terminate){
            pthread_cond_wait(&queue_dequeu_cond,&queue_mutex);

        }else{
            pthread_exit(NULL);
        }
        
    }
    file=a[start];
    start=(start+1)%queue_size;
    pthread_cond_broadcast(&queue_enqueu_cond);
    count--;
    pthread_mutex_unlock(&queue_mutex);
    return file;


}


void* consumer(void* args){
    while(!terminate){
    struct file_info a=dequeue();
    printf("Consumer: consumer have used the file %d with size %d \n",a.file_id,a.size);
    }
    
    return NULL;
}
void *producer(void * args){
    for (int i=0;i<10;i++){
        int file_size=rand()%100;
        enqueue(i,file_size);
        printf("Producer: have stored the file %d with size %d mb\n",i,file_size);
    }
    terminate=1;
    return NULL;
}


int main(){
    pthread_t p,consumer_thread[CONSUMER_THREAD];

    pthread_create(&p,NULL,producer,NULL);
    for (int i=0;i<CONSUMER_THREAD;i++){
        pthread_create(&consumer_thread[i],NULL,consumer,NULL);
    }

    pthread_join(p,NULL);
    for (int i=0;i<CONSUMER_THREAD;i++){
        pthread_join(consumer_thread[i],NULL);
    }


}