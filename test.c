#include <stdio.h>
#include <pthread.h>
#include <unistd.h> 
// void *worker(void *arg)
// {
//     printf("Worker starts\n");
//     sleep(3);
//     printf("Worker finished\n");
//     return NULL;
// }
int counter = 0;
void *worker(void *arg)
{
    for (int i = 0; i < 100000000; i++)
    {
        // usleep(200);
        counter++;
    }

    return NULL;
}
int main()
{
    pthread_t t1;
    pthread_t t2;
    pthread_create(&t2,NULL,worker,NULL);
    pthread_create(&t1, NULL, worker, NULL);
    
    pthread_join(t1, NULL);
    pthread_join(t2,NULL);

    printf("counter = %d\n", counter);
    printf("Main finished\n");

    return 0;
}