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
// gcc -o testIR -Wall -Werror -lcurses -lwiringPi -lpthread -linitio stickyRobot.c
//
//======================================================================

#include <stdlib.h>
#include <initio.h>
#include <curses.h>


//======================================================================
// stickyRobot():
// Extension of testUS() in a way that it follows an object in front of
// it, keeping a certain distance 
//======================================================================
void stickyRobot(int argc, char *argv[]) 
{
  int ch = 0;
  int desiredDis = 20 ; // desired distance to keep from obstacle
  int statusP37 = 0; // status flag for Pin 37
  int timeP37 = 0; // execution counter for control loop
  int timeP37sw = 0; // exec counter of last switch
  pinMode (37, OUTPUT) ; // set Pin 37 to output mode

  while (ch != 'q') {
    int distance = initio_UsGetDistance();
    int err = desiredDis - distance; // control variable to kep track of adherence to distance settings

    mvprintw(1, 1,"%s: Press 'q' to end program", argv[0]);

    //Flashing LED to indicate active program
    timeP37 = millis(); // get current time (unit ms)
    if (timeP37 > (timeP37sw + 500)){
      statusP37 = ! statusP37; // toggle pin P37 value
      digitalWrite (37, statusP37); // set Pin 37 to current value
      timeP37sw = timeP37;
    }

    //Obstacle Detection and Tailing
    if (err<=0 && (initio_IrLeft()==1 || initio_IrRight()==1)){
      initio_DriveForward(60);
      if (initio_IrLeft()==1 && initio_IrRight()==0){
        initio_TurnForward(40, 100);
      }
      else if (initio_IrLeft()==0 && initio_IrRight()==1){
        initio_TurnForward(100, 40);
      }
    }
    else if(initio_IrLeft()==0 && initio_IrRight()==0){ //to find the obstacle when out of range
      initio_DriveForward(60);
    }
    else{
      initio_DriveForward(0);
    }

    ch = getch();
    if (ch != ERR){
      mvprintw(2, 1,"Key code: '%c' (%d)", ch, ch);
      refresh();
      delay (100); // pause 100ms
    }
  }
  return ;
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

  stickyRobot(argc, argv);

  initio_Cleanup ();  // initio: cleanup the library (reset robot car)
  endwin();           // curses: cleanup the library
  return EXIT_SUCCESS;
}
