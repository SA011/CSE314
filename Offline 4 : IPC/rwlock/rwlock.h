#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <iostream>
#include <semaphore.h>

using namespace std;

struct read_write_lock
{
    //mutex: to protect the shared data from concurrent access by multiple writers or readers
    sem_t mutex;

    //reader_count_mutex: to protect the reader_count variable from concurrent access by multiple readers.
    sem_t reader_count_mutex;

    // reader_count: an integer variable that keeps track of the number of readers currently accessing the shared data.
    int reader_count;

    //writer_queue: to control the access to the shared data by writers.
    sem_t writer_queue;
};

void InitalizeReadWriteLock(struct read_write_lock * rw);
void ReaderLock(struct read_write_lock * rw);
void ReaderUnlock(struct read_write_lock * rw);
void WriterLock(struct read_write_lock * rw);
void WriterUnlock(struct read_write_lock * rw);
