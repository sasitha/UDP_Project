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

/*
 * Simple C Test Suite
 */

void test1() {
    printf("simulator test 1\n");
    double delay = 100000;
    int status;
    pid_t pid;
    pthread_mutex_t lock;
    
    pid = fork();
    if(pid == 0){
       pthread_mutex_lock(&lock);
        printf("running server\n");
        system("gnome-terminal --window-with-profile=NAMEOFTHEPROFILE -e ./server  ");
        //system("gnome-terminal");
        //execl("/home/sasitha/semester_06/network_programmming/git_project/UDP_Project/server", "server", (char*)NULL );
        while(delay>=0){
            delay--;
        }
        pthread_mutex_unlock(&lock);
    }else{
        pthread_mutex_lock(&lock);
         printf("running client\n");
        system("gnome-terminal  --window-with-profile=NAMEOFTHEPROFILE -x ./client  127.0.0.1");
        //execl("/home/sasitha/semester_06/network_programmming/git_project/UDP_Project/client", "client", (char*)NULL );
        
        pthread_mutex_unlock(&lock);
        waitpid(-1, &status, 0);
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
