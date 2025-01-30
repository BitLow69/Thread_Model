#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<pthread.h>


#define MASTER 1
#define MANAGER 3
#define WORKER 4
#define QUEQUE_LIMIT 2
#include<stdio.h>



int square(int a){
    return a*a;
}
int cube(int a){
    return a*a*a;
}




struct WorkerDetail{
    int worker_id;
    int (*a[10])(int);
    int work_left;
    pthread_mutex_t lock;

};
struct WorkerDetail *(worker_detail)[MANAGER]; // -> [1,2,3]
void worker_initialize(struct WorkerDetail *worker_detail){
    
    for (int i=0;i<WORKER;i++){
      
        (worker_detail+i)->worker_id=i;
        for (int j=0;j<QUEQUE_LIMIT;j++){
            
            (worker_detail+i)->a[j]=(rand()%100)%2==0?&square:&cube;
        }
        (worker_detail+i)->work_left=QUEQUE_LIMIT;
        pthread_mutex_init(&(worker_detail+i)->lock,NULL);
    


    }

}
struct manager_detail{
    int manager_id;
    int worker_id;
};

void* worker(void *args){


    struct manager_detail *managerial_detail=(struct  manager_detail*)args; 
    int manager_id=managerial_detail-> manager_id;
    int worker_id=managerial_detail->worker_id;
    // to check whether all the task in taht managerial level have beenc completed
    //here we donot need to check again as only once the work is givne once the work is complete no new work is added;
    for (int i=0;i<QUEQUE_LIMIT;i++){
            pthread_mutex_lock(&(worker_detail[manager_id][worker_id]).lock);
            if ((worker_detail[manager_id][worker_id]).work_left<1){ //  this check whether if the worker have teh 
            pthread_mutex_unlock(&worker_detail[manager_id][worker_id].lock);
                break;
            }else{
                int (*a)(int)=(worker_detail[manager_id][worker_id]).a[i];
                printf("Manager%d Worker %d have calcualted task_id %d and perfomr  %d \n",manager_id,worker_id,i,a(rand()%4+1));
                worker_detail[manager_id][worker_id].work_left--;
                pthread_mutex_unlock(&(worker_detail[manager_id][worker_id]).lock);

            }
    }
    
    for (int j = 0; j < WORKER; j++) {
        if (j != worker_id) {
            for (int k = 0; k < QUEQUE_LIMIT; k++) {
                if (pthread_mutex_trylock(&worker_detail[manager_id][j].lock) == 0) {
                    if (worker_detail[manager_id][j].work_left > 0) {
                        int (*task)(int) = worker_detail[manager_id][j].a[k];
                        printf("Worker %d stole Task ID %d from Worker %d and performed %d\n",
                               worker_id, k, j, task(rand() % 4));
                        worker_detail[manager_id][j].work_left--;
                    }
                    pthread_mutex_unlock(&worker_detail[manager_id][j].lock);
                }
            }
        }
    }
    pthread_exit(NULL);

}


void* manager(void *args){
    
    

    int *manger_id =(int *)args;
    pthread_t worker_thread[WORKER];
    worker_detail[*manger_id]=(struct WorkerDetail *)malloc(WORKER*(sizeof(struct WorkerDetail)));



    worker_initialize(worker_detail[*manger_id]);// here all the worker will see  manage as global so it will act as global varialbe for them 

    for (int i=0;i<WORKER;i++){
        struct manager_detail* a=(struct manager_detail*)malloc(sizeof(struct manager_detail));
        a->manager_id=*manger_id;
        a->worker_id=i;
        if ((pthread_create(&worker_thread[i],NULL,worker,a))!=0){
            perror("Error in creating the worker thread\n");
        }

    }

    for (int i=0;i<WORKER;i++){
        if ((pthread_join(worker_thread[i],NULL))!=0){
            perror("Error in joining the  worker thread\n");
        }

    }
    free(worker_detail[*manger_id]);
    free(manger_id);
    return NULL;


}
void* master(void* args){
    pthread_t manager_thread[MANAGER];

    
    for (int i=0;i<MANAGER;i++){
        int *a=(int *)malloc(sizeof(int));
        *a=i;
        if ((pthread_create(&manager_thread[i],NULL,manager,a))!=0){
            perror("Error in creating the thread\n");
        }

    }

    for (int i=0;i<MANAGER;i++){
        if ((pthread_join(manager_thread[i],NULL))!=0){
            perror("Error in joining the thread\n");
        }

    }
    pthread_exit(NULL);

}

int main(){
    srand(time(NULL));
    pthread_t master_thread;
    if ((pthread_create(&master_thread,NULL,master,NULL))!=0){
        perror("error in creating the master thread\n");
    }
    if ((pthread_join(master_thread,NULL))!=0){
        perror("error in joiiing the master thread");
    }

}