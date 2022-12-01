#!/bin/bash

gcc process_test.c -o process_test
gcc thread_test.c -o thread_test
gcc test.c -pthread -o test
gcc test1.c -pthread -o test1
gcc test2.c -pthread -o test2
gcc test3.c -pthread -o test3

