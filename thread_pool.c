#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h>
#include<unistd.h>

#define THREAD_POOL 5

int factorial(int n){
    if (n==1 || n==0){
        return 1;
    }else{
        return (n*factorial(n-1));
    }
}

pthread_mutex_t queue_mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_cond=PTHREAD_COND_INITIALIZER;

//queue for our implementaiton 
struct array_queue{
    int a;
    struct array_queue *node;
};
struct array_queue* header=NULL;
int terminate=0;
int dequue(void){

    pthread_mutex_lock(&queue_mutex);
    struct array_queue* temp;
    
    while(header==NULL && !terminate){
        pthread_cond_wait(&queue_cond,&queue_mutex);
    }
    if (terminate && header==NULL){
        pthread_mutex_unlock(&queue_mutex);
        return -1;

    }
    temp=header;
    header=header->node;
    int value=temp->a;
    free(temp);
    pthread_mutex_unlock(&queue_mutex);
    return value;

}

void enqueu(int number){
    pthread_mutex_lock(&queue_mutex);
    struct array_queue* temp=(struct array_queue*)malloc(sizeof(struct array_queue));
    temp->a=number;
    temp->node=NULL;
    struct array_queue * temp_v=header;
    if (header==NULL){
        header=temp;
    }else{
        while(temp_v->node!=NULL){
            temp_v=temp_v->node;

        }
        temp_v->node=temp;
    }
    pthread_cond_broadcast(&queue_cond);
    pthread_mutex_unlock(&queue_mutex);
    
}
int isempty(void){
    sleep(1);
    pthread_mutex_lock(&queue_mutex);
    struct array_queue* temp=header;
    pthread_mutex_unlock(&queue_mutex);
    return temp==NULL?1:0;
   
}
void print_queue(void){
    struct array_queue* temp=header;
    while(temp!=NULL){
        printf("%d\t",temp->a);
        temp=temp->node;
    }
}


pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;

const char *output_file="output.txt";
pthread_mutex_t output_mutex=PTHREAD_MUTEX_INITIALIZER;
void log_data(int a,int b,unsigned long thread_number){
    pthread_mutex_lock(&output_mutex);
    FILE *ptr=fopen(output_file,"a");
    time_t t=time(NULL);
    char time_buffer[20];
    strftime(time_buffer,sizeof(time_buffer),"%H:%M:%S",localtime(&t));

    fprintf(ptr,"%s : Thread %lu have clauclated the factorial of %d to be %d\n",time_buffer,thread_number,a,b);
    printf("%s : Thread %lu have clauclated the factorial of %d to be %d",time_buffer,thread_number,a,b);
    fclose(ptr);
    pthread_mutex_unlock(&output_mutex);
}

void *calculate_factorial(void *args){
    while(!isempty()){
        int a=dequue();
        int factor=factorial(a);
        unsigned long thread=(unsigned long )pthread_self();
        log_data(a,factor,thread);




    }
    return NULL;
}
void stop_dequeu(void){
    pthread_mutex_lock(&queue_mutex);
    terminate=1;
    pthread_cond_broadcast(&queue_cond);
    pthread_mutex_unlock(&queue_mutex);
}
int main(){
    int a[]={1,2,2,3,4,4,5,5,6,6,7,7,7};
    pthread_t th[THREAD_POOL];
     for (int i=0;i<sizeof(a)/sizeof(int);i++){
        enqueu(a[i]);
    }
    for (int i=0;i<THREAD_POOL;i++){
        pthread_create(&th[i],NULL,calculate_factorial,NULL);

    }
    for (int i=0;i<THREAD_POOL;i++){
        pthread_join(th[i],NULL);
    }
    for (int i=0;i<sizeof(a)/sizeof(int);i++){
        enqueu(a[i]);
    }
    sleep(4);
    stop_dequeu();




}

