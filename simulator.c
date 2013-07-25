/* 
 * File:   simulator.c
 * Author: sasitha
 *
 * Created on July 25, 2013, 11:03 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /* for fork */
#include <sys/types.h> /* for pid_t */
#include <sys/wait.h>
#include <string.h> /* for wait */

/*
 * Simple C Test Suite
 */

void test1() {
    printf("simulator test 1\n");
    double delay = 100000;
    
    
    
    printf("running server\n");
    system("gnome-terminal --window-with-profile=NAMEOFTHEPROFILE -e ./server ");
    while(delay>=0){
        delay--;
    }
    printf("running client\n");
    system("gnome-terminal --window-with-profile=NAMEOFTHEPROFILE -e ./client ");
    
}

void test2() {
    printf("simulator test 2\n");
    printf("%%TEST_FAILED%% time=0 testname=test2 (simulator) message=error message sample\n");
}

int main(int argc, char** argv) {
    printf("%%SUITE_STARTING%% simulator\n");
    printf("%%SUITE_STARTED%%\n");

    printf("%%TEST_STARTED%% test1 (simulator)\n");
    test1();
    printf("%%TEST_FINISHED%% time=0 test1 (simulator) \n");
    
    /*
     *  printf("%%TEST_STARTED%% test2 (simulator)\n");
    test2();
    printf("%%TEST_FINISHED%% time=0 test2 (simulator) \n");

    printf("%%SUITE_FINISHED%% time=0\n");
     */
   

    return (EXIT_SUCCESS);
}
