/* 
 * File:   testing.c
 * Author: sasitha
 *
 * Created on August 19, 2013, 10:37 AM
 */

#include <stdio.h>
#include <stdlib.h>

/*
 * Simple C Test Suite
 */

void test1() {
    printf("testing test 1\n");
}

void test2() {
    printf("testing test 2\n");
    printf("%%TEST_FAILED%% time=0 testname=test2 (testing) message=error message sample\n");
}

int main(int argc, char** argv) {
    printf("%%SUITE_STARTING%% testing\n");
    printf("%%SUITE_STARTED%%\n");

    printf("%%TEST_STARTED%% test1 (testing)\n");
    test1();
    printf("%%TEST_FINISHED%% time=0 test1 (testing) \n");

    printf("%%TEST_STARTED%% test2 (testing)\n");
    test2();
    printf("%%TEST_FINISHED%% time=0 test2 (testing) \n");

    printf("%%SUITE_FINISHED%% time=0\n");

    return (EXIT_SUCCESS);
}
