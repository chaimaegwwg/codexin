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
int ft_time()
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
                printf("time: %ld, id : %d bornout\n",ft_time_opt(cnx[0].data->start_time),cnx[i].id);
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


int can_i_compile(t_coder_context *all_coders, int id, int num)
{
    t_scheduler *data = all_coders[id].data;
    int other_left;
    int other_right;
    // int left_dongle = (id - 1 + num) % num;
    int left_dongle = (id +1 )% num;
    int right_dongle = id % num;
    printf("now it is left %d, right %d\n",left_dongle,right_dongle);
    // if (data->usb[left_dongle] == data->usb[right_dongle])
    // {
    //     printf("time: %ld id: %d has taken a left dongle\n",ft_time_opt(data->start_time),id+1);
    //     return 0;
    // }
    if (data->usb[left_dongle] != 1 || data->usb[right_dongle] != 1)
        return (0);
    int i = 0;
    while(i < num)
    {
        if (i == id)
        {
            i+=1;
            continue;
        }
        if (all_coders[i].is_waiting)
        {
            other_left = (i - 1 + num) % num;
            other_right = i;
            if (other_left == left_dongle || other_left == right_dongle ||
                other_right == left_dongle || other_right == right_dongle)
            {
                printf("the id that it take left :%d, right :%d\n",other_left,other_right);
                if (all_coders[i].request_time < all_coders[id].request_time)
                    return (0); 
            }
        }
        i++;
    }
    return (1);



}
void* ft_coders(void* arg)
{
    int num;
    t_coder_context *ctx = (t_coder_context *)arg;
    t_scheduler *data = ctx->data;
    num = data->number_of_coders;
    int *usb = data->usb;

    int id = ctx->id;
    printf("here the problem %d\n",id);
    printf("check id: %d , %d\n",id,num);
    // printf("id : %d djkfkk\n",id);
    int left = id;
    int right = (id + 1) % num;
    printf("right %d left %d\n",right,left);
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
        
        while(!can_i_compile(data->cnx_array, id, num))
        {
            pthread_cond_wait(&cond,&lock);
        }
        
        // heap_pop(&data->heap);
        ctx->is_waiting = 0;
        usb[left] = 0;
        printf("time: %ld id: %d has taken a left dongle\n",ft_time_opt(data->start_time),id);
        usb[right] = 0;
        printf("time: %ld id: %d has taken a right dongle\n",ft_time_opt(data->start_time),id);
        
        pthread_mutex_unlock(&lock);
     
        usleep(data->time_to_compile * 1000);
        printf("time: %ld id: %d is compiling \n",ft_time_opt(data->start_time),id+1);
        // usleep(data->dongle_cooldown *1000);
        // printf("time: %ld id: %d is compiling \n",ft_time_opt(data->start_time),id);
        pthread_mutex_lock(&lock);
        if (ctx->count_comp < data->number_of_compiles_required)
            ctx->count_comp += 1;
        
        ctx->last_com_time = ft_time();
        // usleep(data->dongle_cooldown);
        usb[left] = 1;
        usb[right] = 1;
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&lock);
        usleep(data->time_to_debug * 1000);
        printf("time: %ld id: %d is debugging\n",ft_time_opt(data->start_time),id);
        usleep(data->time_to_refactor * 1000);
        printf("time: %ld id: %dis refactoring\n",ft_time_opt(data->start_time),id);
    }
    

    
    return NULL;
}
int main_thread(t_scheduler *data)
{
    printf("ddd");
    // printf("num_coder :%d\n time_burn :%d\n time_to_complier: %d\n  debug :%d\n",data->number_of_coders,data->time_to_burnout,data->time_to_compile,data->time_to_debug,data->time_to_refactor,"\n");
    
    int num;
    int i;
    num = data->number_of_coders;
    data->usb = malloc(sizeof(int)*num +1);
    data->queue = malloc(sizeof(int)*num+1);
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
    t_coder_context *cnx = malloc(sizeof(t_coder_context) * num +1);
    if (!cnx)
    return (1);
    data->cnx_array = cnx;
    data->tail = 0;
    
    // pthread_create(&computer,NULL,thread_func_comp,data);
    i = 0;
    while(i < num)
    {
        cnx[i].data = data;
        cnx[i].id = i+1;
        cnx[i].last_com_time = ft_time();
        cnx[i].count_comp = 0;
        i+=1;
    }
    i = 0;
    while (i < num)
    {
        printf("heere");
        pthread_create(&coders[i] ,NULL,ft_coders,&cnx[i]);
        i += 1;
    }
    pthread_create(&monitor_thread, NULL, ft_check_burnout, cnx);
    i = 0;
    while (i <= num)
    {
        pthread_join(coders[i],NULL);
        i += 1;
    }
    free(cnx);
    free(data->usb);
    free(data->queue);

    return 0;
}
