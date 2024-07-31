#include "rwlock.h"

void InitalizeReadWriteLock(struct read_write_lock * rw)
{
  //	Write the code for initializing your read-write lock.
  // This function initializes the reader-writer lock. 
  // It initializes the mutex and reader_count_mutex semaphores with a value of 1 (unlocked), and sets the initial value of reader_count to 0. 
  // It also initializes the writer_queue semaphore with a value of 1.

  //struct tar mutex nam er jinis ta initialize korchi
  //second argument 0 mane hocche semaphore ta holo ekta single argument er thread 
  //third argument 1 mane hocche initial value of the semaphore, mane ekta thread ke allow korboi
  sem_init(&rw->mutex, 0, 1); 
  sem_init(&rw->reader_count_mutex, 0, 1); 
  rw->reader_count=0; 
  sem_init(&rw->writer_queue, 0, 1);
}

void ReaderLock(struct read_write_lock * rw)
{
  //	Write the code for aquiring read-write lock by the reader.
  // This function is called by a reader thread to acquire the lock. 
  // It waits on the reader_count_mutex semaphore to prevent concurrent access to the reader_count variable by multiple readers. 
  // It then increments the reader_count variable and checks if this is the first reader to access the shared data. 
  // If so, it waits on the mutex semaphore to prevent concurrent access by a writer. 
  // Finally, it releases the reader_count_mutex semaphore.
  sem_wait(&rw->reader_count_mutex); 
  rw->reader_count++; 
  if(rw->reader_count==1){
    sem_wait(&rw->mutex); 
  }
  sem_post(&rw->reader_count_mutex); 


}

void ReaderUnlock(struct read_write_lock * rw)
{
  //	Write the code for releasing read-write lock by the reader.
  // This function is called by a reader thread to release the lock. 
  // It waits on the reader_count_mutex semaphore to prevent concurrent access to the reader_count variable by multiple readers. 
  // It then decrements the reader_count variable and checks if this is the last reader to access the shared data. 
  // If so, it releases the mutex semaphore to allow access by a writer. 
  // Finally, it releases the reader_count_mutex semaphore.
  sem_wait(&rw->reader_count_mutex);
  rw->reader_count--; 
  if(rw->reader_count==0)
  {
    sem_post(&rw->mutex);
  }
  sem_post(&rw->reader_count_mutex); 


}

void WriterLock(struct read_write_lock * rw)
{
  //	Write the code for aquiring read-write lock by the writer.
  // This function is called by a writer thread to acquire the lock. 
  // It waits on the writer_queue semaphore to control access to the shared data by multiple writers. 
  // It then waits on the mutex semaphore to prevent concurrent access by a reader or another writer.
  sem_wait(&rw->writer_queue); 
  sem_wait(&rw->mutex); 

}

void WriterUnlock(struct read_write_lock * rw)
{
  //	Write the code for releasing read-write lock by the writer.

  //  increments the value of the mutex semaphore, allowing another thread to acquire it. 
  // This releases the lock held by the writer.
  sem_post(&rw->mutex); 

  //   increments the value of the writer_queue semaphore, 
  //allowing another writer thread to wait in the queue to acquire the lock.
  sem_post(&rw->writer_queue); 

}
