#include <iostream>
#include <thread>
#include <vector>
#include <memory>

#include <semaphore.h>

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

static timespec resource_time;
static timespec rmutex_time;
static timespec order_time;

static unsigned int readCount = 0;

static void writer(std::vector<int>& vec) {
    std::cout << '\n';

    for(int i = 0; i < ITERS; ++i) {
        if(sem_timedwait(&order, &order_time) == 0) {
        	printf("Writer [%lld] is waiting on the %d iteration\n",
					std::this_thread::get_id(), i);
		}

        sem_timedwait(&resource, &resource_time);
        sem_post(&order);

        printf("Writer [%lld] is working...\n", 
				std::this_thread::get_id());

        vec.push_back(1);

        sem_post(&resource);

        printf("Data has been written successfully [%lld]\n",
		        std::this_thread::get_id());
    }
}

static void reader(std::vector<int>& vec)
{
	std::string vec_str;

    std::cout << '\n';
    for(int i = 0; i < ITERS; ++i)
    {
        if(sem_timedwait(&order, &order_time) == 0) {
			printf("Reader [%lld] is waiting on the %ld iteration...\n",
					std::this_thread::get_id(), i);
		}

        sem_timedwait(&rmutex, &rmutex_time);

        if(readCount == 0)
            sem_timedwait(&resource, &resource_time);

        ++readCount;

        sem_post(&order);
        sem_post(&rmutex);

		for (const auto& el : vec) {
			vec_str += std::to_string(el);
			vec_str += ", ";
		}

        printf("Reader [%lld] is working...\n\t",
		        std::this_thread::get_id());

        sem_timedwait(&rmutex, &rmutex_time);

        --readCount;

        if(readCount == 0)
            sem_post(&resource);

        sem_post(&rmutex);

        printf("Data has been read successfully [%lld]: [%s]\n",
		        std::this_thread::get_id(),
		        vec_str.c_str());

    }
}

int main()
{
    sem_init(&resource, 0, 1);
    sem_init(&rmutex, 0, 1);
    sem_init(&order, 0, 1);

    std::vector<int> vec;
    vec.reserve(ITERS * WRITERS);

    resource_time.tv_sec = RESOURCE_TIME * 1000;
    resource_time.tv_nsec = 0;

    rmutex_time.tv_sec = RMUTEX_TIME * 1000;
    rmutex_time.tv_nsec = 0;

    order_time.tv_sec = ORDER_TIME * 1000;
    order_time.tv_nsec = 0;

    std::unique_ptr<std::thread[]> writers_threads(new std::thread[WRITERS]);
    std::unique_ptr<std::thread[]> readers_threads(new std::thread[READERS]);

    std::cout << '\n';

    for(int i = 0; i < WRITERS; ++i) {
        writers_threads[i] = std::thread(writer, std::ref(vec));
    }

    for(int i = 0; i < READERS; ++i) {
        readers_threads[i] = std::thread(reader, std::ref(vec));
    }

    for(int i = 0; i < WRITERS; ++i) {
        if(writers_threads[i].joinable()) {
            writers_threads[i].join();
        }
    }

    for(int i = 0; i < READERS; ++i) {
        if(readers_threads[i].joinable()) {
            readers_threads[i].join();
        }
   	}

    std::cout << '\n';

    sem_destroy(&resource);
    sem_destroy(&rmutex);
    sem_destroy(&order);

    return 0;
}
