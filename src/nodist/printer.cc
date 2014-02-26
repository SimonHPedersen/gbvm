#include <iostream.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/times.h>

#define ITERATIONS 10000

int main(int argc, char **argv) {
  struct tms before;
  struct tms after;

  if((clock_t)-1 == times(&before)) {
    cerr << "GarbageCollector_t::garbageCollect(): error getting time\n";
    exit(1);
  }


  double gc_worst_time = 0, gc_avg_time = 0, gctimes = 0;

  while(gctimes < ITERATIONS) {
    struct tms beforeGCTime, afterGCTime;
    
    if((clock_t)-1 == times(&beforeGCTime)) {
      cerr << "GarbageCollector_t::garbageCollect(): error getting time\n";
      exit(1);
    }

    if((clock_t)-1 == times(&afterGCTime)) {
      cerr << "GarbageCollector_t::garbageCollect(): error getting time\n";
      exit(1);
    }
    
    int tics = afterGCTime.tms_utime - beforeGCTime.tms_utime;
    double secs = ((double)tics)/CLK_TCK;
    
    cout << "\ngc took: " << tics << " ticks ~ " << secs << " seconds\n";
    
    if(gc_worst_time < secs) gc_worst_time = secs;
    
    gc_avg_time = (gc_avg_time * gctimes + secs) / (gctimes + 1);
    
    ++gctimes;
    
    cout << "Average is: " << gc_avg_time << " secs, worst is: " << gc_worst_time << endl;
  }
  cout << "\ngc's: " << gctimes << " avg. time: " << gc_avg_time
       << " sec, worst: " << gc_worst_time << " sec, total: "
       << gctimes*gc_avg_time << endl;



  if((clock_t)-1 == times(&after)) {
    cerr << "GarbageCollector_t::garbageCollect(): error getting time\n";
    exit(1);
  }

  int time_in_tics = after.tms_utime - before.tms_utime;
  double time_in_secs = ((double)time_in_tics)/CLK_TCK;

  cout << "\nONE_PRINTOUT_TAKES: " << time_in_secs/ITERATIONS << endl;
  return 0;
}

