/*
Simulate a banking system with a shared account balance.//done
Create 5 threads representing customers.//done

Each thread performs 10 random transactions://done
Deposit a random amount between $1 and $100.
Withdraw a random amount between $1 and $100.
Ensure no overdrafts (no withdrawals beyond the current balance).
Start with an initial balance of $1000.
Log each transaction:
Thread ID, transaction type (deposit or withdraw), amount, and resulting balance.
*/

//requried librarries

#include<stdio.h>
#include<unistd.h>
#include<pthread.h>
#include<stdlib.h>
#include<time.h>

int shared_account_balance=1000;

#define THREAD_POOL 5
#define TRANSACTIONS 10

const char* const output_file="outputfile.txt";

//mutex and conditonal for deposti and withdraw
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond=PTHREAD_COND_INITIALIZER;

void logg(unsigned long ,char *,int ,int );

//deposit and withdraw functions
void deposit(){
    int random=rand()%100+1;
    pthread_mutex_lock(&mutex);
    shared_account_balance+=random;
    pthread_cond_broadcast(&cond);
    logg((unsigned long)pthread_self(),"deposit",random,shared_account_balance);
    pthread_mutex_unlock(&mutex);

}
void withdraw(){
    int random=rand()%100+1;
    pthread_mutex_lock(&mutex);
    while(random>shared_account_balance){
        pthread_cond_wait(&cond,&mutex);
    }
    shared_account_balance-=random;
    logg((unsigned long)pthread_self(),"withdraw",random,shared_account_balance);
    pthread_mutex_unlock(&mutex);


}


//function to log it shoudl log time, thread_which create the operition, deposit or withdraw 

pthread_mutex_t file_mutex=PTHREAD_MUTEX_INITIALIZER;
void logg(unsigned long thread,char *opearation,int value,int shared_account){
    char time_buffer[200];
    time_t t=time(NULL);
    strftime(time_buffer,sizeof(time_buffer),"%D:%H:%M:%S",localtime(&t));
    FILE *ptr=fopen(output_file,"a");
    pthread_mutex_lock(&file_mutex);
    fprintf(ptr,"%s : Thread %lu have performed %s operaion with balance %d. The final balance is %d\n",time_buffer,thread,opearation,value,shared_account);
    fclose(ptr);
    pthread_mutex_unlock(&file_mutex);
    
}


void *transaction(void *args){
    void (*a[3])()={&deposit,&withdraw,&deposit};
    for (int i=0;i<TRANSACTIONS;i++){
        int task=rand()%3;
        a[task]();
    }
    return NULL;
}

int main(void){
    pthread_t customer[THREAD_POOL];
    for (int i=0;i<THREAD_POOL;i++){
        pthread_create(&customer[i],NULL,transaction,NULL);

    }
    for (int i=0;i<THREAD_POOL;i++){
        pthread_join(customer[i],NULL);
    }


}

