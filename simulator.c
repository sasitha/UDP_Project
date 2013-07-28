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
#include <pthread.h>

#include "headsock.h"

/*
 * Simple C Test Suite
 */

void test1() {
    printf("simulator test 1\n");
    double delay = 10;
    char *client_str, *server_str;
    int count = 5;
    float error_ratio = 0.01;
    char *temp;
    while(error_ratio<=0.1 && count >=0){
        count--;
        error_ratio = error_ratio*10; 
        client_str = "gnome-terminal  --window-with-profile=NAMEOFTHEPROFILE -x ./client  127.0.0.1";
        server_str = "gnome-terminal --window-with-profile=NAMEOFTHEPROFILE -x ./server ";
        sprintf(temp, "%lld", error_ratio);
        printf("%s\n", server_str);
        printf("error ratio %0.3f\n", error_ratio);
        printf("temp %s\n", temp);
        //strcat(server_str, temp );
       // printf("starting server\n");
       // system(server_str);
        while(delay>=0){
                delay--;
        }
        //printf("starting client\n");
       // system(client_str);
        
    }
    
    
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
