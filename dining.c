/*
 * OS Assignment #3
 *
 * @file dininig.c
 */

#include "sem.h"
#include <stdio.h>
#include <unistd.h>
#include <sched.h>

static tsem_t *chopstick[5];
static tsem_t *printing;

static void
update_status (int i,
	       int eating)
{
  static int status[5] = { 0, };
  int idx;

  status[i] = eating;

  tsem_wait (printing);
  for (idx = 0; idx < 5; idx++)
    fprintf (stdout, "%3s     ", status[idx] ? "EAT" : "...");
  fprintf (stdout, "\n");
  tsem_signal (printing);
}

void *
thread_func (void *arg)
{
  int i = (int) (long) arg;

  do
  {
	  // 좌우 두개 젓가락 모두 사용가능 할 때만 젖가락을 집는다.
	  while(1)
	  {
		  if(tsem_try_wait(chopstick[i]) != 0)
		  {
			  continue;
		  }

		  if(tsem_try_wait(chopstick[(i + 1) % 5]) == 0)
		  {
			  break;
		  }

		  // 왼쪽 젖가락을 사용 할 수 없으면 오른쪽 젖가락을 내려 놓는다.
		  tsem_signal (chopstick[i]);
	  }

	  update_status (i, 1);
	  tsem_signal (chopstick[i]);
	  tsem_signal (chopstick[(i + 1) % 5]);
	  update_status (i, 0);
  }
  while (1);

  return NULL;
}

int
main (int    argc,
      char **argv)
{
  int i;

  for (i = 0; i < 5; i++)
    chopstick[i] = tsem_new (1);
  printing = tsem_new (1);

  for (i = 0; i < 5; i++)
    {
      pthread_t tid;

      pthread_create (&tid, NULL, thread_func, (void *) (long) i);
    }

  /* endless thinking and eating... */
  while (1)
    usleep (10000000);
  
  return 0;
}
