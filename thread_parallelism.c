#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t queue_cond = PTHREAD_COND_INITIALIZER;

struct array_queue {
    int a;
    struct array_queue *node;
};
struct array_queue* header = NULL;
int terminate = 0;

int dequue(void) {
    pthread_mutex_lock(&queue_mutex);
    while (header == NULL && !terminate) {
        pthread_cond_wait(&queue_cond, &queue_mutex);
    }
    if (terminate && header == NULL) {
        pthread_mutex_unlock(&queue_mutex);
        return -1; // No tasks left, terminate thread
    }
    struct array_queue* temp = header;
    header = header->node;
    int value = temp->a;
    free(temp);
    pthread_mutex_unlock(&queue_mutex);
    return value;
}

void enqueu(int number) {
    pthread_mutex_lock(&queue_mutex);
    struct array_queue* temp = malloc(sizeof(struct array_queue));
    temp->a = number;
    temp->node = NULL;

    if (header == NULL) {
        header = temp;
    } else {
        struct array_queue* current = header;
        while (current->node != NULL) {
            current = current->node;
        }
        current->node = temp;
    }

    pthread_cond_broadcast(&queue_cond); // Notify all threads
    pthread_mutex_unlock(&queue_mutex);
}

void stop_dequeu(void) {
    pthread_mutex_lock(&queue_mutex);
    terminate = 1;
    pthread_cond_broadcast(&queue_cond); // Wake up all waiting threads
    pthread_mutex_unlock(&queue_mutex);
}

void* dynamic_input(void* args) {
    char input[100];
    while (1) {
        printf("Enter a number (or 'exit' to quit): ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0'; // Remove newline character

        if (strcmp(input, "exit") == 0) {
            stop_dequeu();
            break;
        }

        char* end;
        long value = strtol(input, &end, 10);
        if (end == input || *end != '\0') {
            printf("Invalid input. Please enter a number.\n");
            continue;
        }

        enqueu((int)value);
    }
    return NULL;
}

void* execute_task(void* args) {
    while (1) {
        int task = dequue();
        if (task == -1) {
            break; // Exit the thread if no tasks are left
        }
        printf("Thread %lu processed task: %d\n", (unsigned long)pthread_self(), task);
        sleep(1); // Simulate task processing time
    }
    return NULL;
}

int main(void) {
    pthread_t input_thread;
    pthread_create(&input_thread, NULL, dynamic_input, NULL);

    pthread_t execute[5];
    for (int i = 0; i < 5; i++) {
        pthread_create(&execute[i], NULL, execute_task, NULL);
    }

    pthread_join(input_thread, NULL); // Wait for the input thread to finish

    for (int i = 0; i < 5; i++) {
        pthread_join(execute[i], NULL); // Wait for worker threads to finish
    }

    printf("All tasks processed. Exiting program.\n");
    return 0;
}
