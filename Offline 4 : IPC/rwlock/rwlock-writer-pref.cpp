#include "rwlock.h"

void InitalizeReadWriteLock(struct read_write_lock * rw)
{
  //	Write the code for initializing your read-write lock.

  //rw er mutex nam er jinis ta ekta semaphore, er value 1 set kore nicchi first e 
  //0 mane ekta process er multiple thread er jonno 
  //1 mane unlock
  sem_init(&rw->mutex, 0, 1);

  //reader count variable ta access er jonno lock use korlam
  //reader countMutex 1 mane ekhon je keu eita access korte parbe
  sem_init(&rw->reader_count_mutex, 0, 1);

  //surute kono reader nai
  rw->reader_count = 0;

  //writer_queue keu access korte parbe bole, er value 1 set kore dilam
  sem_init(&rw->writer_queue, 0, 1);

}

void ReaderLock(struct read_write_lock * rw)
{
  //	Write the code for aquiring read-write lock by the reader.
  //ek jon reader ashche
  //keu jodi write korte thake tahole rw->mutex er value 0 mane eke wait korte hobe

  //Writer preference bole eita korlam
  sem_wait(&rw->writer_queue);
  //reader count er mutex ta onno keu access korche kina totokkhon just ektu wait kore thako tumi
  sem_wait(&rw->reader_count_mutex);

  //ini reader bole reader er count 1 baralam 
  rw->reader_count++;
  if (rw->reader_count == 1) {
    sem_wait(&rw->mutex);
  }

  sem_post(&rw->reader_count_mutex);

  //Writer preference bole eita korlam; mane ekhon writer queue te arekjon dhukte parbe
  sem_post(&rw->writer_queue);


}

void ReaderUnlock(struct read_write_lock * rw)
{
  //	Write the code for releasing read-write lock by the reader.
  sem_wait(&rw->reader_count_mutex);
  rw->reader_count--;
  if (rw->reader_count == 0) {
    sem_post(&rw->mutex);
  }
  sem_post(&rw->reader_count_mutex);


}

void WriterLock(struct read_write_lock * rw)
{
  //	Write the code for aquiring read-write lock by the writer.
  sem_wait(&rw->writer_queue);
  sem_wait(&rw->mutex);


}

void WriterUnlock(struct read_write_lock * rw)
{
  //	Write the code for releasing read-write lock by the writer.
  sem_post(&rw->mutex);
  sem_post(&rw->writer_queue);

}
