#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#define THREAD_COUNT 4
#define SHOP_COUNT 5

pthread_t tid[THREAD_COUNT];
pthread_mutex_t mutex[SHOP_COUNT];
int shop[SHOP_COUNT], departed_buyers;

struct Args {
    int id;
};

void *buyer(void *args)
{
    struct Args *thread = (struct Args *)args;
    int select = 0, need = 10000;
    while (need > 0) {
        select = rand() % 5;
        if (pthread_mutex_trylock(&mutex[select]) != EBUSY) {
            sleep(2);
            printf("thread (%d): buyer enter in %d shop (need = %d, product = %d)\n", thread->id, select + 1, need, shop[select]);
            if (need - shop[select] >= 0) {
                need -= shop[select];
                shop[select] = 0;
            } else {
                shop[select] -= need;
                need = 0;
                departed_buyers++;
            }
            pthread_mutex_unlock(&mutex[select]);
        }
    }

    return 0;
}

void *loader(void *args)
{
    int select = 0;
    while (departed_buyers != THREAD_COUNT - 1) {
        select = rand() % 5;
        if (pthread_mutex_trylock(&mutex[select]) != EBUSY) {
            sleep(1);
            shop[select] += 500;
            printf("loader enter in %d shop and loaded products (total = %d)\n", select + 1, shop[select]);
            pthread_mutex_unlock(&mutex[select]);
        }
    }

    return 0;
}

int main(void)
{
    struct Args thread[THREAD_COUNT - 1];

    for (int i = 0; i < SHOP_COUNT; i++) {
        pthread_mutex_init(&mutex[i], NULL);
        shop[i] = rand() % 21 + 490;
    }
    departed_buyers = 0;

    for (int i = 0; i < THREAD_COUNT - 1; i++) {
        thread[i].id = i + 1;
        pthread_create(&tid[i], NULL, buyer, (void *)&thread[i]);
    }
    pthread_create(&tid[THREAD_COUNT - 1], NULL, loader, NULL);
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(tid[i], NULL);
    }

    for (int i = 0; i < SHOP_COUNT; i++) {
        pthread_mutex_destroy(&mutex[i]);
    }

    exit(EXIT_SUCCESS);
}
