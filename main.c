#include <stdio.h>
#include <stdlib.h>

#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/queue.h>

//num of iterations
//of each thread
#define ITERS 10

//num of readers and writers
#define READERS 5
#define WRITERS 5

//waiting time
#define RESOURCE_TIME 5 //seconds for resource
#define RMUTEX_TIME 5 //seconds for rmutex
#define ORDER_TIME 5 //seconds for order

static sem_t resource;
static sem_t rmutex;
static sem_t order;

static struct timespec resource_time;
static struct timespec rmutex_time;
static struct timespec order_time;

static unsigned int rdcnt = 0;

struct entry {
	int data;
	SLIST_ENTRY(entry) entries;
};

SLIST_HEAD(slisthead, entry);

static void* writer(void* arg) {
	struct slisthead *head = (struct slisthead*) arg;

	putchar('\n');

    for(int i = 0; i < ITERS; ++i) {
        if(sem_timedwait(&order, &order_time) == 0) {
        	printf("Writer is waiting on the %d iteration\n", i);
		}

        sem_timedwait(&resource, &resource_time);
        sem_post(&order);

        printf("Writer is working...\n");

        struct entry* el = malloc(sizeof(struct entry));
        el->data = 1;

        SLIST_INSERT_HEAD(head, el, entries);

        sem_post(&resource);

        printf("Data has been written successfully\n");
    }
}

static void* reader(void* arg) {
	struct slisthead *head = (struct slisthead*) arg;

	putchar('\n');

    for(int i = 0; i < ITERS; ++i)
    {
        if(sem_timedwait(&order, &order_time) == 0) {
			printf("Reader is waiting on the %d iteration...\n", i);
		}

        sem_timedwait(&rmutex, &rmutex_time);

        if(rdcnt == 0)
            sem_timedwait(&resource, &resource_time);

        ++rdcnt;

        sem_post(&order);
        sem_post(&rmutex);

        printf("Reader is working...\n\t");

        sem_timedwait(&rmutex, &rmutex_time);

        --rdcnt;

        if(rdcnt == 0)
            sem_post(&resource);

        sem_post(&rmutex);

        printf("Data has been read successfully\n");
    }
}

int main() {
	pthread_t readers_id[READERS];
	pthread_t writers_id[WRITERS];

    struct slisthead head;
    struct entry* ep;

	SLIST_INIT(&head);

    sem_init(&resource, 0, 1);
    sem_init(&rmutex, 0, 1);
    sem_init(&order, 0, 1);

    resource_time.tv_sec = RESOURCE_TIME * 1000;
    resource_time.tv_nsec = 0;

    rmutex_time.tv_sec = RMUTEX_TIME * 1000;
    rmutex_time.tv_nsec = 0;

    order_time.tv_sec = ORDER_TIME * 1000;
    order_time.tv_nsec = 0;

    putchar('\n');

    for(int i = 0; i < WRITERS; ++i) {
        pthread_create(&writers_id[i], NULL, writer, &head);
    }

    for(int i = 0; i < READERS; ++i) {
        pthread_create(&readers_id[i], NULL, reader, &head);
    }

    for(int i = 0; i < WRITERS; ++i) {
	    pthread_join(writers_id[i], NULL);
    }

    for(int i = 0; i < READERS; ++i) {
	    pthread_join(readers_id[i], NULL);
   	}

    putchar('\n');

    sem_destroy(&resource);
    sem_destroy(&rmutex);
    sem_destroy(&order);

	while (!SLIST_EMPTY(&head)) {
		ep = SLIST_FIRST(&head);
		SLIST_REMOVE_HEAD(&head, entries);

		free(ep);
	}

	exit(EXIT_SUCCESS);
}

