#ifndef HEADER_H
#define HEADER_H
#include <pthread.h>
typedef struct s_scheduler
{
    int number_of_coders;
    long time_to_burnout;
    long time_to_compile;
    long time_to_debug;
    long time_to_refactor;
    long  dongle_cooldown;
    int number_of_compiles_required;
    int is_edf;
    int *turn;
    int tail;
    int head;
    int *usb;
    int *queue;
    long start_time;
    pthread_mutex_t log_lock;
    // pthread_mutex_t lock;              //  g_lock
    // pthread_mutex_t log_lock;          //  log_lock
    // pthread_cond_t  cond;              //  g_cond
    // long            sequence_counter;  //  g_sequence_counter
    struct s_coder_context *cnx_array;
} t_scheduler;

typedef struct s_coder_context
{
    t_scheduler *data; 
    int         id; 
    long last_com_time;
    long count_comp;
    long            request_time;     
    int             is_waiting;
    
} t_coder_context;
int main_thread(t_scheduler *data);
#endif



















// #ifndef HEADER_H
// #define HEADER_H
// #include <pthread.h>

// typedef struct s_scheduler
// {
//     int number_of_coders;
//     long time_to_burnout;
//     long time_to_compile;
//     long time_to_debug;
//     long time_to_refactor;
//     long  dongle_cooldown;
//     int number_of_compiles_required;
//     int *turn;
//     int tail;
//     int is_edf;
//     int head;
//     int *usb;
//     int *queue;
//     long start_time;
//     struct s_coder_context *cnx_array;
// } t_scheduler;
// typedef struct s_coder_context
// {
//     t_scheduler *data; 
//     int         id; 
//     int last_com_time;
//     long count_comp;
// } t_coder_context;
// int main_thread(t_scheduler *data);
// #endif