/*
 * File:         generic.c
 * Description:  This is an empty robot controller, the robot does nothing.
 * Author:       www.cyberbotics.com
 * Note:         !!! PLEASE DO NOT MODIFY THIS SOURCE FILE !!!
 *               This is a system file that Webots needs to work correctly.
 *
 * TEMPORARY DEBUG VERSION - Added debugging for troubleshooting crashes
 */

#include <webots/robot.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

// Signal handler for debugging crashes
void crash_handler(int sig) {
  printf("DEBUG: Controller received signal %d (crash/termination)\n", sig);
  fflush(stdout);
  exit(sig);
}

int main() {
  // Install signal handlers for debugging
  signal(SIGSEGV, crash_handler);
  signal(SIGABRT, crash_handler);
  signal(SIGTERM, crash_handler);
  signal(SIGINT, crash_handler);
  
  printf("DEBUG: Generic controller starting...\n");
  fflush(stdout);
  
  printf("DEBUG: Calling wb_robot_init()...\n");
  fflush(stdout);
  
  wb_robot_init();
  
  printf("DEBUG: wb_robot_init() completed successfully\n");
  fflush(stdout);
  
  printf("DEBUG: Getting basic time step...\n");
  fflush(stdout);
  
  int time_step = wb_robot_get_basic_time_step();
  
  printf("DEBUG: Basic time step = %d\n", time_step);
  fflush(stdout);
  
  if (time_step == 0) {
    printf("DEBUG: Time step was 0, setting to 1\n");
    fflush(stdout);
    time_step = 1;
  }
  
  printf("DEBUG: Entering main loop with time_step = %d\n", time_step);
  fflush(stdout);
  
  int step_count = 0;
  for (;;) {
    printf("DEBUG: Step %d - calling wb_robot_step(%d)...\n", step_count++, time_step);
    fflush(stdout);
    
    int result = wb_robot_step(time_step);
    
    printf("DEBUG: wb_robot_step returned %d\n", result);
    fflush(stdout);
    
    // Check if Webots wants to terminate the controller
    if (result == -1) {
      printf("DEBUG: wb_robot_step returned -1, exiting...\n");
      fflush(stdout);
      break;
    }
    
    // Limit debug output after first few steps
    if (step_count > 5) {
      // Only print every 100 steps to avoid spam
      if (step_count % 100 == 0) {
        printf("DEBUG: Controller still running, step %d\n", step_count);
        fflush(stdout);
      }
    }
  }
  
  printf("DEBUG: Main loop exited, controller terminating normally\n");
  fflush(stdout);
  
  return 0;
}
