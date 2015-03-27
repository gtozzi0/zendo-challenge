#include <stdio.h>
#include <sys/time.h>

/* Note: this doesn't handle rollover for our stack timestamp priorities, but
 * should suffice as an example. */

long get_ms_timestamp(void)
{
  struct timeval tv;

  gettimeofday(&tv, NULL);

  return ( (tv.tv_sec * 1000) + (tv.tv_usec / 1000) );
}

#ifdef TIME_TEST
int main(void)
{
  long milliseconds;

  while(1)
  {
    milliseconds = get_ms_timstamp();
    printf("%ld\n", milliseconds);
  }

  return 0;
}
#endif

