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
    t_scheduler *data; 
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
            pthread_mutex_lock(&lock);
            if (ft_time() - cnx[i].last_com_time > bournout)
            {
                printf("time: %lld, bornout\n",ft_time_opt(cnx[0].data->start_time));
                exit(0);
            }
            if (cnx[i].count_comp >= cnx[i].data->number_of_compiles_required)
                full_coders++;
                i +=1;
        }
        if (full_coders == num) {
            printf("time: %lld, All required compiles successfully!\n",ft_time_opt(cnx[0].data->start_time));
            exit(0); 
        }
        pthread_mutex_unlock(&lock);
        usleep(500);
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
    int left = id;
    int right = (id + 1) % num ;
    int i = 0;
    while(1)
    {
        pthread_mutex_lock(&lock);
        if (ctx->count_comp >= data->number_of_compiles_required)
        {
            ctx->last_com_time = ft_time(); 
            pthread_mutex_unlock(&lock);
            usleep(500); 
            continue; 
        }
        if ((usb[left] != 1) || (usb[right] != 1) || ((data->head % num) != (data->tail % num)))
        {
            data->queue[data->tail % num] = id;
            data->tail = (data->tail + 1) % num;
            while((usb[left % num] != 1) || (usb[right % num] != 1) || (id != data->queue[data->head]))
            {
                pthread_cond_wait(&cond,&lock);
            }
            data->head = (data->head + 1 )% num;
        }
        
        if ((data->turn[left % num] == 0) && (data->turn[right % num] == 0))
        {
            printf("time: %lld, id: %d it didin't\n",ft_time_opt(data->start_time),id);
        }
        else
        {
            usleep(data->dongle_cooldown);
            printf("time: %lld, id: %d it waitting-----------> \n",ft_time_opt(data->start_time),id);
        }
        data->turn[left % num] = 1;
        data->turn[right % num] = 1;
        
        usb[left] = 0;
        usb[right] = 0;
        printf("time: %lld, id: %d has taken a dongle\n",ft_time_opt(data->start_time),id);
        pthread_mutex_unlock(&lock);
        // if (ft_time() - ctx->last_com_time > data->time_to_burnout)
        // {
        //     printf("%d Nooooooooooo -------> burnout",id);
        // }
        usleep(data->time_to_compile);
        printf("time: %lld, id: %d is compiling \n",ft_time_opt(data->start_time),id);
        if (ctx->count_comp < data->number_of_compiles_required)
            ctx->count_comp += 1;
        pthread_mutex_lock(&lock);
        ctx->last_com_time = ft_time();
        // usleep(data->dongle_cooldown);
        usb[left] = 1;
        usb[right] = 1;
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&lock);
        printf("time: %lld, id: %d is debugging\n",ft_time_opt(data->start_time),id);
        usleep(data->time_to_refactor);
        printf("time: %lld , id: %dis refactoring\n",ft_time_opt(data->start_time),id);
    }
    

    
    return NULL;
}
int main_thread(t_scheduler *data)
{
    // printf("djkfkk\n");
    // printf("num_coder :%d\n time_burn :%d\n time_to_complier: %d\n  debug :%d\n",data->number_of_coders,data->time_to_burnout,data->time_to_compile,data->time_to_debug,data->time_to_refactor,"\n");
    int num;
    int i;
    num = data->number_of_coders;
    data->usb = malloc(sizeof(int)*num);
    data->queue = malloc(sizeof(int)*num);
    data->start_time = ft_time();
    data->head = 0;
    i = 0;
    while(i <num)
    {
        data->queue[i] = 0;
        data->usb[i] = 1;
        data->turn[i] = 0;
        i +=1;
    }
  
    pthread_t coders[num];
    pthread_t monitor_thread;
    t_coder_context cnx[num];
    data->tail = 0;
    
    // pthread_create(&computer,NULL,thread_func_comp,data);
    i = 0;
    while (i < num)
    {
        cnx[i].data = data;
        cnx[i].id = i;
        cnx[i].last_com_time = ft_time();
        cnx[i].count_comp = 0;
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

    return 0;
}
