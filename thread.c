#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "header.h"
#include <sys/time.h>

// number_of_compiles_required
//  mission now :fifo or edf
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
long long ft_time()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}
long ft_time_opt(long time)
{
    int time_now = 0;
    time_now = ft_time() - time;
    return time_now;
}
void pre_sleep(long long time_in_ms)
{
    long long start = ft_time();

    while ((ft_time() - start) < time_in_ms)
    {
        if (time_in_ms - (ft_time() - start) > 1)
            usleep(500); 
        else
            usleep(50);
    }
}
void pre_usleep(long long time_to_sleep)
{
    long long start = ft_time();
    // time_to_sleep = ft_time_opt()
    // printf("hello %lld time :%lld      , start is : %lld\n",(ft_time() +  time_to_sleep ),ft_time(), start);
    while(ft_time() - start <  time_to_sleep)
    {
        // printf("ji %lld time : %lld \n",ft_time() - start, time_to_sleep);
        // printf("hello %ld time :%ld      \n",ft_time_opt(ft_time() - start) ,ft_time_opt(time_to_sleep));
        // pthread_mutex_unlock(&lock);
        usleep(500);
    }
}
void* ft_check_burnout(void*arg)
{
    // t_scheduler *data;
    int num;
    t_coder_context *cnx = (t_coder_context *)arg;
    long bournout;
    bournout = cnx[0].data->time_to_burnout;
    num = cnx[0].data->number_of_coders;
    
    int i;
    int full_coders = 0;
    while(1)
    {
        i = 0;
        full_coders = 0;
        while (i < num)
        {
            // printf("here the proble\n");
            pthread_mutex_lock(&lock);
            if (ft_time() - cnx[i].last_com_time > bournout)
            {
                printf("time: %ld, id : %d bornout\n",ft_time_opt(cnx[0].data->start_time),cnx[i].id+1);
                exit(0);
            }
            
            if (cnx[i].count_comp >= cnx[i].data->number_of_compiles_required)
                full_coders++;
            pthread_mutex_unlock(&lock);
            i +=1;
        }
        pthread_mutex_lock(&lock);
        if (full_coders == num) {
            printf("time: %ld, All required compiles successfully!\n",ft_time_opt(cnx[0].data->start_time));
            exit(0); 
        }
        pthread_mutex_unlock(&lock);
        usleep(500);
    }
    return NULL;
}
int is_neighbor_eligible(t_scheduler *data, int neighbor_id, int num)
{
    int neighbor_left_dongle = neighbor_id;
    int neighbor_right_dongle = (neighbor_id + 1) % num;

    if (data->usb[neighbor_left_dongle] == 1 && data->usb[neighbor_right_dongle] == 1)
        return (1);
    return (0); 
}

int can_i_compile_edf(t_coder_context *all_coders, int id, int num)
{
    t_scheduler *data = all_coders[id].data;
    int left_dongle = id;
    int right_dongle = (id + 1) % num;

    int left_neighbor = (id + num - 1) % num;
    int right_neighbor = (id + 1) % num;
    
    if (left_dongle == right_dongle)
    {
        printf("time: %ld id: %d has taken a left dongle\n",ft_time_opt(data->start_time),id+1);
        return (0);
    }

    if (data->usb[left_dongle] != 1 || data->usb[right_dongle] != 1)
        return (0);


  
    if (all_coders[left_neighbor].is_waiting)
    {
        if (is_neighbor_eligible(data, left_neighbor, num))
        {
            if (all_coders[left_neighbor].last_com_time < all_coders[id].last_com_time)
                return (0);
        }
    }

    if (all_coders[right_neighbor].is_waiting)
    {
        if (is_neighbor_eligible(data, right_neighbor, num))
        {
            if (all_coders[right_neighbor].last_com_time < all_coders[id].last_com_time)
                return (0);
        }
    }

    return (1);
}
int can_i_compile_fifo(t_coder_context *all_coders, int id, int num)
{
    t_scheduler *data = all_coders[id].data;
    // int other_left;
    // int other_right;
    int left_dongle = id;
    int right_dongle = (id + 1) % num;
    int left_neighbor = (id + num - 1) % num;
    int right_neighbor = (id + 1) % num;
    if (num == 0)
        return (0);
    if (left_dongle == right_dongle)
    {
        printf("time: %ld id: %d has taken a left dongle\n",ft_time_opt(data->start_time),id+1);
        return (0);
    }
    if (data->usb[left_dongle] != 1 || data->usb[right_dongle] != 1)
        return (0);

    if (all_coders[left_neighbor].is_waiting)
    {
        if (is_neighbor_eligible(data, left_neighbor, num))
        {
            if (all_coders[left_neighbor].request_time < all_coders[id].request_time)
                return (0);
        }
    }

    if (all_coders[right_neighbor].is_waiting)
    {
        if (ft_time() - all_coders[id].last_compile_end < data->dongle_cooldown)
        return (0);
        if (is_neighbor_eligible(data, right_neighbor, num))
        {
            if (all_coders[right_neighbor].request_time < all_coders[id].request_time)
                return (0);
        }
    }
   
    return (1);



}
int can_i_compile(t_coder_context *all_coders, int id, int num)
{
    t_scheduler *data = all_coders[id].data;

    if(!(data->is_edf))
    {
        return (can_i_compile_fifo(data->cnx_array, id, num));
    }
    else
    {
        return (can_i_compile_edf(data->cnx_array, id, num));
    }
}
void* ft_coders(void* arg)
{
    int num;
    t_coder_context *ctx = (t_coder_context *)arg;
    t_scheduler *data = ctx->data;
    num = data->number_of_coders;
    int *usb = data->usb;

    int id = ctx->id;
    // printf("here the problem %d\n",id);
    // printf("check id: %d , %d\n",id,num);
    // printf("id : %d djkfkk\n",id);
    int left = id;
    int right = (id + 1) % num;
    // printf("right %d left %d\n",right,left);
    // int i = 0;
    while(1)
    {
        // printf("here we go\n");
        pthread_mutex_lock(&lock);
        // if (data->usb[left] != data->usb[right])
        // {
        if (ctx->count_comp >= data->number_of_compiles_required)
        {
            ctx->last_com_time = ft_time(); 
            pthread_mutex_unlock(&lock);
            usleep(500); 
            continue; 
        }
        
        // long req_time = ft_time();
        // heap_push(&data->heap, id, req_time);
        
        // data->queue[data->tail % num] = id;
        // data->tail = (data->tail + 1) % num;
        ctx->request_time = ft_time();
        
        ctx->is_waiting = 1;
        while(!(can_i_compile(data->cnx_array, id, num)))
        {
            pthread_cond_wait(&cond,&lock);
        }
        
        // heap_pop(&data->heap);
        ctx->is_waiting = 0;
        usb[left] = 0;
        printf("time: %ld id: %d has taken a left dongle\n",ft_time_opt(data->start_time),id+1);
        usb[right] = 0;
        printf("time: %ld id: %d has taken a right dongle\n",ft_time_opt(data->start_time),id+1);
        ctx->last_com_time = ft_time();
        printf("time: %ld id: %d is compiling \n",ft_time_opt(data->start_time),id+1);
        pthread_mutex_unlock(&lock);
        // usleep(data->dongle_cooldown *1000);
        // printf("time: %ld id: %d is compiling \n",ft_time_opt(data->start_time),id);
        
        usleep(data->time_to_compile*1000);
        
        pthread_mutex_lock(&lock);
        
        if (ctx->count_comp < data->number_of_compiles_required)
            ctx->count_comp += 1;
        ctx->last_com_time = ft_time();
        pthread_mutex_unlock(&lock);
        // usleep(data->dongle_cooldown);
        // if (data->dongle_cooldown > 0)
        //     usleep(data->dongle_cooldown * 1000);
        pthread_mutex_lock(&lock);
        usb[left] = 1;
        usb[right] = 1;
        ctx->last_compile_end = ft_time();
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&lock);
        pthread_mutex_lock(&lock);
        printf("time: %ld id: %d is debugging\n",ft_time_opt(data->start_time),id+1);
        pthread_mutex_unlock(&lock);
        usleep(data->time_to_debug*1000);
        pthread_mutex_lock(&lock);
        printf("time: %ld id: %dis refactoring\n",ft_time_opt(data->start_time),id+1);
        pthread_mutex_unlock(&lock);
        usleep(data->time_to_refactor*1000);
    }
    

    
    return NULL;
}
int main_thread(t_scheduler *data)
{
    
    int num;
    int i;
    num = data->number_of_coders;
    data->usb = malloc(sizeof(int)*num);
    data->queue = malloc(sizeof(int)*num);
    data->start_time = ft_time();
    // data->head = 0;
    // data->heap.tree = malloc(sizeof(t_heap_node) * data->number_of_coders);
    // data->heap.size = 0;
    // pthread_mutex_init(&data->log_lock, NULL);
    i = 0;
    // printf("%d\n",num);
    while(i < num)
    {
        data->queue[i] = 0;
        data->usb[i] = 1;
        i +=1;
    }
    pthread_t coders[num];
    pthread_t monitor_thread;
    t_coder_context *cnx = malloc(sizeof(t_coder_context) * num);
    if (!cnx)
    return (1);
    data->cnx_array = cnx;
    data->tail = 0;
    
    // pthread_create(&computer,NULL,thread_func_comp,data);
    i = 0;
    while(i < num)
    {
        cnx[i].data = data;
        cnx[i].id = i;
        cnx[i].last_com_time = ft_time();
        cnx[i].count_comp = 0;
        cnx[i].is_waiting = 0;
        cnx[i].last_compile_end=0;
        i+=1;
    }
    i = 0;
    while (i < num)
    {
        // printf("heere");
        pthread_create(&coders[i] ,NULL,ft_coders,&cnx[i]);
        i += 1;
    }
    pthread_create(&monitor_thread, NULL, ft_check_burnout, cnx);
    i = 0;
    while (i < num)
    {
        pthread_join(coders[i],NULL);
        i += 1;
    }
    free(cnx);
    free(data->usb);
    free(data->queue);

    return 0;
}
