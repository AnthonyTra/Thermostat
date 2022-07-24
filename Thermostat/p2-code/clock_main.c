// DO NOT MODIFY
// 
// LCD Clock simulator. Read a # of seconds from the beginning of the
// day from the command line and show the results of running functions
// from clock_update.c on the screen. Makes use of functions from
// clock_clock.c

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "clock.h"

int main(int argc, char **argv){

  if(argc < 2){                 // Set TIME_OF_DAY_SEC to the actual time
    time_t rawtime;             // Get raw time with system calls
    time( &rawtime );
    struct tm *info;              // Store broken down time
    info = localtime( &rawtime ); // Get a broken down time

    // Calculate the seconds since the beginning of the day
    TIME_OF_DAY_SEC = info->tm_sec + 60*info->tm_min + 60*60*info->tm_hour;
  }
  else{                         // Set time based on argument given
    TIME_OF_DAY_SEC = atoi(argv[1]);
  }
  printf("TIME_OF_DAY_SEC set to: %d\n", TIME_OF_DAY_SEC);

  int time_max = 24 * 60 * 60;
  if(TIME_OF_DAY_SEC >= time_max){
    printf("Time %d exceeds max %d\n",TIME_OF_DAY_SEC,time_max);
    return -1;
  }

  tod_t tod;
  int result;

  printf("result = set_tod_from_secs( %5d, &tod );\n",TIME_OF_DAY_SEC);
  result = set_tod_from_secs(TIME_OF_DAY_SEC, &tod);
  printf("result: %d\n",result);
  printf("tod = {        \n"); 
  printf("  .hours   = %d\n",tod.hours);
  printf("  .minutes = %d\n",tod.minutes);
  printf("  .seconds = %d\n",tod.seconds);
  printf("  .ampm    = %d\n",tod.ampm);
  printf("}              \n");
  printf("Simulated time is: %02d : %02d : %02d %s\n",
         tod.hours,tod.minutes,tod.seconds,(tod.ampm==1 ? "am" : "pm"));

  printf("%s", (result == 0) ? "" : "WARNING: Non-zero value returned\n");

  int display = 0;
  printf("result = set_display_from_tod(tod, &display);\n");
  result = set_display_from_tod(tod, &display);
  printf("result: %d\n", result);
  printf("display is\n");
  printf("bits:  %s\n",bitstr(display, INT_BITS));
  printf("index: %s\n",bitstr_index(INT_BITS));

  printf("%s", (result == 0) ? "" : "WARNING: Non-zero value returned\n");

  printf("\n");

  printf("result = clock_update();\n");
  result = clock_update();
  printf("result: %d\n", result);
  printf("CLOCK_DISPLAY_PORT is\n");
  printf("bits:  %s\n",bitstr(CLOCK_DISPLAY_PORT, INT_BITS));
  printf("index: %s\n",bitstr_index(INT_BITS));

  printf("%s", (result == 0) ? "" : "WARNING: Non-zero value returned\n");

  printf("\n");

  printf("Clock Display:\n");
  print_clock_display();

  return 0;
}
