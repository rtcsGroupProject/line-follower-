//======================================================================
//
// Test program to test the infrared sensors (and motors) of the
// 4tronix initio robot car. One can run this program within an
// ssh session.
//
// author: Raimund Kirner, University of Hertfordshire
//         initial version: Oct.2016
//
// license: GNU LESSER GENERAL PUBLIC LICENSE
//          Version 2.1, February 1999
//          (for details see LICENSE file)
//
// Compilation: 
// gcc -o testIR -Wall -Werror -lcurses -lwiringPi -lpthread -linitio testIR.c
//
//======================================================================

#include <stdlib.h>
#include <initio.h>
#include <curses.h>

//======================================================================
// testIR():
// Simple program to test infrared obstacle sensors:
// Drive forward and stop whenever an obstacle is detected by either
// the left or right infrared (IR) sensor.
//======================================================================
void testIR(int argc, char *argv[]) 
{
  int ch = 0;
  int distance = initio_UsGetDistance();
  int threshold = 50; //Value to adjust the distance at which the robot car changes direction
  int speed = 70;
  int statusP37 = 0; // status flag for Pin 37
  int timeP37 = 0; // execution counter for control loop
  int timeP37sw = 0; // exec counter of last switch
  pinMode (37, OUTPUT) ; // set Pin 37 to output mode

  while (ch != 'q') {
    mvprintw(1, 1,"%s: Press 'q' to end program", argv[0]);

  //Flashing LED to indicate active program
    timeP37 = millis(); // get current time (unit ms)
    if (timeP37 > (timeP37sw + 500)){
      statusP37 = ! statusP37; // toggle pin P37 value
      digitalWrite (37, statusP37); // set Pin 37 to current value
      timeP37sw = timeP37;
    }

    /*Tutorial 2 EX. 5 or Tutorial 3 EX. 1- Change Direction when close to an object 
    Incorporates Tutorial 3 EX. 2 & EX. 3 - Slows down by 70% once it detects an obstacle within 100cm 
                                            & Stops once the obstacles is within 50cm which is set as the threshold and when it detects a dark area on the ground*/
    if (distance <= threshold)
    {
      //starts driving forward at a reduced speed
      if(distance <= 100){
        double reducedSpeed = 0.3 * speed;
        initio_DriveForward(reducedSpeed);
      } 

      if (initio_IrLeft() && initio_IrRight()==0){
        initio_DriveForward(0);
        initio_SpinRight(100); 
      } 
      else if(initio_IrRight() && initio_IrLeft()==0){
        initio_DriveForward(0);
        initio_SpinLeft(100); 
      } 
      else if(initio_IrRight() && initio_IrLeft()){
        initio_DriveForward(0);
	      initio_SpinRight(100);
      } 

      if(initio_IrLineLeft() || initio_IrLineRight()){
        initio_DriveForward(0);
      }
    }
    else if (initio_IrLeft()==0 && initio_IrRight()==0){
      initio_DriveForward (speed);
    }
  }
   
    ch = getch();
    if (ch != ERR){
    mvprintw(2, 1,"Key code: '%c' (%d)", ch, ch);
    refresh();
    delay (100); // pause 100ms
    }
  return;
}






//======================================================================
// main(): initialisation of libraries, etc
//======================================================================
int main (int argc, char *argv[])
{
  WINDOW *mainwin = initscr();  // curses: init screen
  noecho ();                    // curses: prevent the key being echoed
  cbreak();                     // curses: disable line buffering 
  nodelay(mainwin, TRUE);       // curses: set getch() as non-blocking 
  keypad (mainwin, TRUE);       // curses: enable detection of cursor and other keys

  initio_Init (); // initio: init the library

  testIR(argc, argv);

  initio_Cleanup ();  // initio: cleanup the library (reset robot car)
  endwin();           // curses: cleanup the library
  return EXIT_SUCCESS;
}

