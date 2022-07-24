#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "clock.h"

#define CLOCK_MAX_BITS 30

// Global variable used to control the LCD display on the
// clock. Making changes to this variable will change the clock
// time. Type ensures 32 bits.
int32_t CLOCK_DISPLAY_PORT;

// Time of day in seconds. Tied to a hardware clock that automatically
// increments it 1 time per second starting at midnight. The
// simulatator provides a C global variable to emulate this part of
// the hardware. This variable is present when #include "clock.h" is
// used and should not be declared in any user code.
int TIME_OF_DAY_SEC;

#define NROWS 7
#define NCOLS 24+1

// Convenience struct for representing the battery display  as characters 
typedef struct {
  char chars[NROWS][NCOLS];
} clock_display;

clock_display init_display = {
  .chars = {
    "                        ",
    "                        ",
    "          o             ",
    "                        ",
    "          o             ",
    "                        ",
    "                        ",
  }
};

// Reset an clock_display to be empty 
void reset_clock_display(clock_display *clock){
  *clock = init_display;
}  

// Print an clock_display 
void internal_print_clock_display(clock_display *clock){
  int i=0;
  for(i=0; i<NROWS; i++){
    printf("%s\n",clock->chars[i]);
  }
}  

// Data and functions to set the state of the lcd clock display 

// Position and char in display 
typedef struct {
  int r,c; char ch;
} charpos;
    
// Collection of characters corresponding to one bit in the state being set 
typedef struct {
  int len;                      // number of chars corresponding to this bit
  charpos pos[5];               // position of chars for this bit
} charpos_coll;

#define TOP 0
#define LFT 0
#define LMD 5
#define RMD 12
#define RGT 17

// Correspondence of bit positions to which characters should be set 
charpos_coll bits2chars[CLOCK_MAX_BITS] = {
  // RGT
  { .len=4, .pos={{ 0 +TOP, 0 +RGT,'#'},{ 0 +TOP, 1 +RGT,'#'}, { 0 +TOP, 2 +RGT,'#'},{ 0 +TOP, 3 +RGT,'#'}} }, //  0 top
  { .len=4, .pos={{ 0 +TOP, 0 +RGT,'#'},{ 1 +TOP, 0 +RGT,'#'}, { 2 +TOP, 0 +RGT,'#'},{ 3 +TOP, 0 +RGT,'#'}} }, //  1 left upper
  { .len=4, .pos={{ 3 +TOP, 0 +RGT,'#'},{ 4 +TOP, 0 +RGT,'#'}, { 5 +TOP, 0 +RGT,'#'},{ 6 +TOP, 0 +RGT,'#'}} }, //  2 left lower
  { .len=4, .pos={{ 6 +TOP, 0 +RGT,'#'},{ 6 +TOP, 1 +RGT,'#'}, { 6 +TOP, 2 +RGT,'#'},{ 6 +TOP, 3 +RGT,'#'}} }, //  3 bottom
  { .len=4, .pos={{ 3 +TOP, 3 +RGT,'#'},{ 4 +TOP, 3 +RGT,'#'}, { 5 +TOP, 3 +RGT,'#'},{ 6 +TOP, 3 +RGT,'#'}} }, //  4 right lower
  { .len=4, .pos={{ 0 +TOP, 3 +RGT,'#'},{ 1 +TOP, 3 +RGT,'#'}, { 2 +TOP, 3 +RGT,'#'},{ 3 +TOP, 3 +RGT,'#'}} }, //  5 right upper
  { .len=4, .pos={{ 3 +TOP, 0 +RGT,'#'},{ 3 +TOP, 1 +RGT,'#'}, { 3 +TOP, 2 +RGT,'#'},{ 3 +TOP, 3 +RGT,'#'}} }, //  6 middle
  // RMD
  { .len=4, .pos={{ 0 +TOP, 0 +RMD,'#'},{ 0 +TOP, 1 +RMD,'#'}, { 0 +TOP, 2 +RMD,'#'},{ 0 +TOP, 3 +RMD,'#'}} }, //  7 top
  { .len=4, .pos={{ 0 +TOP, 0 +RMD,'#'},{ 1 +TOP, 0 +RMD,'#'}, { 2 +TOP, 0 +RMD,'#'},{ 3 +TOP, 0 +RMD,'#'}} }, //  8 left upper
  { .len=4, .pos={{ 3 +TOP, 0 +RMD,'#'},{ 4 +TOP, 0 +RMD,'#'}, { 5 +TOP, 0 +RMD,'#'},{ 6 +TOP, 0 +RMD,'#'}} }, //  9 left lower
  { .len=4, .pos={{ 6 +TOP, 0 +RMD,'#'},{ 6 +TOP, 1 +RMD,'#'}, { 6 +TOP, 2 +RMD,'#'},{ 6 +TOP, 3 +RMD,'#'}} }, // 10 bottom
  { .len=4, .pos={{ 3 +TOP, 3 +RMD,'#'},{ 4 +TOP, 3 +RMD,'#'}, { 5 +TOP, 3 +RMD,'#'},{ 6 +TOP, 3 +RMD,'#'}} }, // 11 right lower
  { .len=4, .pos={{ 0 +TOP, 3 +RMD,'#'},{ 1 +TOP, 3 +RMD,'#'}, { 2 +TOP, 3 +RMD,'#'},{ 3 +TOP, 3 +RMD,'#'}} }, // 12 right upper
  { .len=4, .pos={{ 3 +TOP, 0 +RMD,'#'},{ 3 +TOP, 1 +RMD,'#'}, { 3 +TOP, 2 +RMD,'#'},{ 3 +TOP, 3 +RMD,'#'}} }, // 13 middle
  // LMD
  { .len=4, .pos={{ 0 +TOP, 0 +LMD,'#'},{ 0 +TOP, 1 +LMD,'#'}, { 0 +TOP, 2 +LMD,'#'},{ 0 +TOP, 3 +LMD,'#'}} }, // 14 top
  { .len=4, .pos={{ 0 +TOP, 0 +LMD,'#'},{ 1 +TOP, 0 +LMD,'#'}, { 2 +TOP, 0 +LMD,'#'},{ 3 +TOP, 0 +LMD,'#'}} }, // 15 left upper
  { .len=4, .pos={{ 3 +TOP, 0 +LMD,'#'},{ 4 +TOP, 0 +LMD,'#'}, { 5 +TOP, 0 +LMD,'#'},{ 6 +TOP, 0 +LMD,'#'}} }, // 16 left lower
  { .len=4, .pos={{ 6 +TOP, 0 +LMD,'#'},{ 6 +TOP, 1 +LMD,'#'}, { 6 +TOP, 2 +LMD,'#'},{ 6 +TOP, 3 +LMD,'#'}} }, // 17 bottom
  { .len=4, .pos={{ 3 +TOP, 3 +LMD,'#'},{ 4 +TOP, 3 +LMD,'#'}, { 5 +TOP, 3 +LMD,'#'},{ 6 +TOP, 3 +LMD,'#'}} }, // 18 right lower
  { .len=4, .pos={{ 0 +TOP, 3 +LMD,'#'},{ 1 +TOP, 3 +LMD,'#'}, { 2 +TOP, 3 +LMD,'#'},{ 3 +TOP, 3 +LMD,'#'}} }, // 19 right upper
  { .len=4, .pos={{ 3 +TOP, 0 +LMD,'#'},{ 3 +TOP, 1 +LMD,'#'}, { 3 +TOP, 2 +LMD,'#'},{ 3 +TOP, 3 +LMD,'#'}} }, // 20 middle
  // LFT
  { .len=4, .pos={{ 0 +TOP, 0 +LFT,'#'},{ 0 +TOP, 1 +LFT,'#'}, { 0 +TOP, 2 +LFT,'#'},{ 0 +TOP, 3 +LFT,'#'}} }, // 21 top
  { .len=4, .pos={{ 0 +TOP, 0 +LFT,'#'},{ 1 +TOP, 0 +LFT,'#'}, { 2 +TOP, 0 +LFT,'#'},{ 3 +TOP, 0 +LFT,'#'}} }, // 22 left upper
  { .len=4, .pos={{ 3 +TOP, 0 +LFT,'#'},{ 4 +TOP, 0 +LFT,'#'}, { 5 +TOP, 0 +LFT,'#'},{ 6 +TOP, 0 +LFT,'#'}} }, // 23 left lower
  { .len=4, .pos={{ 6 +TOP, 0 +LFT,'#'},{ 6 +TOP, 1 +LFT,'#'}, { 6 +TOP, 2 +LFT,'#'},{ 6 +TOP, 3 +LFT,'#'}} }, // 24 bottom
  { .len=4, .pos={{ 3 +TOP, 3 +LFT,'#'},{ 4 +TOP, 3 +LFT,'#'}, { 5 +TOP, 3 +LFT,'#'},{ 6 +TOP, 3 +LFT,'#'}} }, // 25 right lower
  { .len=4, .pos={{ 0 +TOP, 3 +LFT,'#'},{ 1 +TOP, 3 +LFT,'#'}, { 2 +TOP, 3 +LFT,'#'},{ 3 +TOP, 3 +LFT,'#'}} }, // 26 right upper
  { .len=4, .pos={{ 3 +TOP, 0 +LFT,'#'},{ 3 +TOP, 1 +LFT,'#'}, { 3 +TOP, 2 +LFT,'#'},{ 3 +TOP, 3 +LFT,'#'}} }, // 27 middle
  // AM / PM
  { .len=2, .pos={{ 5,22,'A'}, { 5,23,'M'},} }, // 28
  { .len=2, .pos={{ 6,22,'P'}, { 6,23,'M'},} }, // 29

};

// Assumes ints are at least 32 bits 
void set_clock_display_state(clock_display *clock, int state){
  int i,j;
  int mask = 0x1;
  reset_clock_display(clock);
  for(i=0; i<CLOCK_MAX_BITS; i++){
    //    printf("Checking %d\n",i);
    if( state & (mask << i) ){ // ith bit set, fill in characters 
      //      printf("%d IS SET\n",i);
      charpos_coll coll = bits2chars[i];
      //      printf("Coll len: %d\n",coll.len);
      for(j=0; j<coll.len; j++){
        //        printf("Inner iter %d\n",j);
        charpos pos = coll.pos[j];
        clock->chars[pos.r][pos.c] = pos.ch;
        // print_clock_display(clock);
      }
    }
  }
}


// Use the global CLOCK_DISPLAY_PORT to print the time 
void print_clock_display(){
  clock_display clock;
  set_clock_display_state(&clock, CLOCK_DISPLAY_PORT);
  internal_print_clock_display(&clock);
  return;
}

#define NCLUSTERS 6
int clusters[NCLUSTERS] = {
  2, 2, 7, 7, 7, 7,
};

// Generate a string version of the bits which clusters the bits based
// on the logical groupings in the problem
char *bitstr(int x, int bits){
  static char buffer[512];
  int idx = 0;
  int clust_idx = 0;
  int clust_break = clusters[0];
  for(int i=0; i<bits; i++){
    if(i==clust_break){
      buffer[idx] = ' ';
      idx++;
      clust_idx++;
      clust_break += clusters[clust_idx];
    }
    buffer[idx] = x & (1 << (31-i)) ? '1' : '0';
    idx++;
  }
  buffer[idx] = '\0';
  return buffer;
}

// Creates a string of bit indices matching the clustering pattern
// above
char *bitstr_index(int bits){
  static char buffer[512];
  char format[256];
  int pos = 0;
  int idx = bits;
  for(int i=0; i<NCLUSTERS; i++){
    idx -= clusters[i];
    if(clusters[i] > 1){
      sprintf(format, "%s%dd ","%",clusters[i]);
      pos += sprintf(buffer+pos, format, idx);
    }
    else{                       // cluster of 1 bit gets no index
      pos += sprintf(buffer+pos, "  ");
    }
  }
  buffer[pos-1] = '\0';         // eliminate trailing space
  return buffer;
}
