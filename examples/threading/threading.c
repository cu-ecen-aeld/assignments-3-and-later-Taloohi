#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
// Optional: use these functions to add debug or error prints to your application
// #define DEBUG_LOG(msg,...)
#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

// pthread_mutex_t mutex;

void* threadfunc(void* thread_param)
{

    //obtain mutex here
    

    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    struct thread_data* thread_func_args = (struct thread_data *) thread_param;
    printf("Thread ID: %d, Message: %s\n", thread_func_args->thread_id, thread_func_args->thread_message);
    
    usleep(thread_func_args->wait_to_obtain_ms*1000);
    pthread_mutex_lock(thread_func_args->mutex);

    usleep(thread_func_args->wait_to_release_ms*1000);
    // release mutex

    pthread_mutex_unlock(thread_func_args->mutex);

    thread_func_args->thread_complete_success = true;

    // free(thread_func_args);

    return thread_param;
}


bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex, int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */

    // sleep(wait_to_obtain_ms);
    
    struct thread_data* thread_data_ = (struct thread_data*)malloc(sizeof(struct thread_data));

    thread_data_->mutex = mutex;
    thread_data_->wait_to_obtain_ms = wait_to_obtain_ms;
    thread_data_->wait_to_release_ms = wait_to_release_ms;
    thread_data_->thread_complete_success = false;

    int passed = pthread_create(thread, NULL, threadfunc, thread_data_);    

    if (thread_data_ == NULL){
        return false;
    }

    // sleep(wait_to_release_ms);

    if (passed == 0){
        return true;
        free(thread_data_);
        thread_data_=NULL;
    }
    else{
        return false;
        free(thread_data_);
        thread_data_=NULL;
    }

    return false;
}

 