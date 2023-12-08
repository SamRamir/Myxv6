include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define BSIZE 10
#define MAX 20
#define NULL 0

typedef struct
{
    int buf[BSIZE];
    int nextin;
    int nextout;
    int num_produced;
    int num_consumed;
    int total;
    sem_t occupied;
    sem_t free;
    sem_t lock;
} buffer_t;

buffer_t *buffer;

void producer()
{
    while (1)
    {
        sem_wait(&buffer->free);
        sem_wait(&buffer->lock);
        if (buffer->num_produced >= MAX)
        {
            sem_post(&buffer->free);
            sem_post(&buffer->occupied);
            sem_post(&buffer->lock);
            exit(0);
        }
        buffer->num_produced++;
        printf("producer %d producing %d\n", getpid(), buffer->num_produced);
        buffer->buf[buffer->nextin++] = buffer->num_produced;

        buffer->nextin %= BSIZE;
        sem_post(&buffer->occupied);
        sem_post(&buffer->lock);
    }
}

void consumer()
{
    while (1)
    {
        sem_wait(&buffer->occupied);
        sem_wait(&buffer->lock);
        if (buffer->num_consumed >= MAX)
        {
            sem_post(&buffer->occupied);
            sem_post(&buffer->free);
            sem_post(&buffer->lock);
            exit(0);
        }
        printf("consumer %d consuming %d\n", getpid(), buffer->buf[buffer->nextout]);
        buffer->total += buffer->buf[buffer->nextout++];
        buffer->nextout %= BSIZE;
        buffer->num_consumed++;
        sem_post(&buffer->free);
        sem_post(&buffer->lock);
    }
}

void run_testcase(int nproducers, int nconsumers)
{
    for (int i = 0; i < BSIZE; i++)
        buffer->buf[i] = 0;

    for (int i = 0; i < nconsumers; i++)
        if (!fork())
        {
            consumer();
            exit(0);
        }
    for (int i = 0; i < nproducers; i++)
        if (!fork())
        {
            producer();
            exit(0);
        }
    for (int i = 0; i < nconsumers + nproducers; i++)
        wait(0);
    printf("total = %d\n", buffer->total);
    sem_destroy(&buffer->occupied);
    sem_destroy(&buffer->free);
    sem_destroy(&buffer->lock);
    munmap(buffer, sizeof(buffer_t));
}

int main(void)
{
    buffer = (buffer_t *)mmap(NULL, sizeof(buffer_t),
                               PROT_READ | PROT_WRITE,
                               MAP_ANONYMOUS | MAP_SHARED, -1, 0);

    sem_init(&buffer->occupied, 1, 0);
    sem_init(&buffer->free, 1, BSIZE);
    sem_init(&buffer->lock, 1, 1);

    // Test case 1
    buffer->nextin = 0;
    buffer->nextout = 0;
    buffer->num_produced = 0;
    buffer->num_consumed = 0;
    buffer->total = 0;
    run_testcase(1, 1);

    // Test case 2
    buffer->nextin = 0;
    buffer->nextout = 0;
    buffer->num_produced = 0;
    buffer->num_consumed = 0;
    buffer->total = 0;
    run_testcase(2, 3);

    // Test case 3
    buffer->nextin = 0;
    buffer->nextout = 0;
    buffer->num_produced = 0;
    buffer->num_consumed = 0;
    buffer->total = 0;
    run_testcase(3, 5);

    // Test case 4
    buffer->nextin = 0;
    buffer->nextout = 0;
    buffer->num_produced = 0;
    buffer->num_consumed = 0;
    buffer->total = 0;
    run_testcase(4, 6);

    exit(0);
}
