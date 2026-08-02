// #include <pthread.h>
// #include <stdio.h>
// #include <unistd.h>

// pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
// pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

// int food_ready = 0;
// int turn = 1;

// void *chef(void *arg)
// {
//     printf("Chef: cooking...\n");
//     sleep(3);

//     pthread_mutex_lock(&lock);

//     food_ready = 1;
//     printf("Chef: food is ready!\n");

//     pthread_cond_broadcast(&cond);

//     pthread_mutex_unlock(&lock);

//     return NULL;
// }

// void *client(void *arg)
// {
//     int id = *(int *)arg;

//     pthread_mutex_lock(&lock);

//     while (!food_ready || turn != id)
//         pthread_cond_wait(&cond, &lock);

//     printf("Client %d: got food\n", id);

//     turn++;

//     pthread_cond_broadcast(&cond);

//     pthread_mutex_unlock(&lock);

//     return NULL;
// }

// int main()
// {
//     pthread_t chef_thread;
//     pthread_t clients[4];

//     int ids[4] = {1, 2, 3, 4};

//     pthread_create(&chef_thread, NULL, chef, NULL);

//     for (int i = 0; i < 4; i++)
//         pthread_create(&clients[i], NULL, client, &ids[i]);

//     pthread_join(chef_thread, NULL);

//     for (int i = 0; i < 4; i++)
//         pthread_join(clients[i], NULL);

//     return 0;
// }