/*
 * OS Assignment #3
 *
 * @file sem.c
 */

#include "sem.h"
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>

struct test_semaphore
{
	// 세마포어 접근의 원자성을 확보하기 위해 뮤텍스를 사용한다.
	pthread_mutex_t mutex;

	// condition wait 을 제어하기 위한 변수
	pthread_cond_t cond;

	// 세마포어 값
	int val;
};

tsem_t *
tsem_new (int value)
{
	// 메모리 할당
	tsem_t* s = malloc(sizeof(tsem_t));
	if(s == NULL) return NULL;

	// mutex 초기화
	if(pthread_mutex_init(&s->mutex, NULL) != 0)
	{
		free(s);
		return NULL;
	}

	// pthread_cond 초기화
	if(pthread_cond_init(&s->cond, NULL) != 0)
	{
		pthread_mutex_destroy(&s->mutex); 
		free(s);
		return NULL;
	}

	// 세마포어 값 초기화
	s->val = value;
	return s;
}

void
tsem_free (tsem_t *sem)
{
	if(sem == NULL) return;

	// 뮤텍스는 락이 잡혀 있을 수 있으므로 한번 잡았다 풀고 디스트로이 한다.
	pthread_mutex_lock(&sem->mutex);
	pthread_mutex_unlock(&sem->mutex);
	pthread_mutex_destroy(&sem->mutex); 
	pthread_cond_destroy(&sem->cond);

	// 메모리 free
	free(sem);
}

void
tsem_wait (tsem_t *sem)
{
	assert(sem != NULL);

	// 뮤텍스 락
	pthread_mutex_lock(&sem->mutex);

	while(1)
	{
		// 세마포어 값이 0보다 큰경우 1 감소 후 리턴
		if(sem->val > 0)
		{
			sem->val--;
			pthread_mutex_unlock(&sem->mutex);
			return;
		}

		// 세마포어 값이 0인 경우signal 을 받을 때까지 wait!!
		pthread_cond_wait(&sem->cond, &sem->mutex);
	}
}

int
tsem_try_wait (tsem_t *sem)
{
	assert(sem != NULL);

	// 뮤텍스 락
	pthread_mutex_lock(&sem->mutex);

	// 세마포어 값이 0보다 큰경우 1 감소 후 리턴
	if(sem->val > 0)
	{
		sem->val--;
		pthread_mutex_unlock(&sem->mutex);
		return 0;
	}

	// 세마포어 값이 0인 경우 1 리턴
	pthread_mutex_unlock(&sem->mutex);
  	return 1;
}

void tsem_signal (tsem_t *sem)
{
	assert(sem != NULL);

	// 뮤텍스 락
	pthread_mutex_lock(&sem->mutex);

	// 세마포어 값을 1 올리고 wait 하고 있는 쓰레드에게 알린다.
	sem->val++;

	// signal
	pthread_cond_signal(&sem->cond);

	pthread_mutex_unlock(&sem->mutex);
}










