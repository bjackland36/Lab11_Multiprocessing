/*
File: mandelMovie.c
Author: Blake Ackland
Assignment: Lab 11 - Multiprocessing
Course: CPE2600 - 131
Instructor: Dr. Turney
Date: 11/17/2026
Description: This program repeatedly uses mandel.c to create mandelbroc
             images to then be compiled together to make a movie. The
             program takes the number of processes as an argument to
             determine how many children processes will be used.
Compile: gcc -Wall -g mandelMovie.c -o mandelMovie -lm
Run: "./mandelMovie" or "./mandelMovie -p <processes>"
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/wait.h>

//Determine number of images to make from this program
#define NUM_IMAGES 50

void show_help();

int main(int argc, char *argv[])
{
    //Initialize variables
    char c;
    pid_t pid;
    char scale_str[32];
    char file_str[32];
    char thread_str[32];

    int max_processes = 1;  //Default processes = 1
    int active_processes = 0;
    int max_threads = 1;
    double initial_scale = 4.0;
    double final_scale = 0.0005;

    //Handle command-line args
    while ((c = getopt(argc, argv, "p:t:h")) != -1)
    {
        switch (c)
        {
            case 'p':
                max_processes = atoi(optarg);
                break;
            case 't':
                max_threads = atoi(optarg);
                break;
            case 'h':
                show_help();
                exit(0);
            case '?':
                show_help();
                exit(1);
        }
    }

    //Handle command-line misinputs
    if (max_processes < 1)
    {
        max_processes = 1;
    }
    
    if (max_threads < 1)
    {
        max_threads = 1;
    }
    if (max_threads > 20)
    {
        max_threads = 20;
    }

    //For every image...
    for (int i = 0; i < NUM_IMAGES; i++)
    {
        if (active_processes >= max_processes)
        {
            wait(NULL);
            active_processes--;
        }

        pid = fork();
        if (pid < 0) //Fork failed
        {
            perror("fork");
            exit(1);
        }

        if (pid == 0) //Child process
        {
            //Change scale geometrically based on the current image
            double scale = initial_scale * pow(final_scale / initial_scale,
                (double)i / (NUM_IMAGES - 1));

            //Turn calculated scale and new image name into strings for argument
            snprintf(scale_str, sizeof(scale_str), "%.10f", scale);
            snprintf(file_str, sizeof(file_str), "mandel%d.jpg", i + 1);
            snprintf(thread_str, sizeof(thread_str), "%d", max_threads);

            //Create args to invoke with ./mandel
            char *args[] = {"./mandel", "-x", "-0.088", "-y", "0.654", "-s",
                scale_str, "-m", "500", "-o", file_str, "-t", thread_str, NULL};

            //run ./mandel via this program
            if (execv("./mandel", args) == -1)
            {
                perror("execv");
                exit(1);
            }
        }
        //iterate active processes
        active_processes++;
    }

    while (active_processes > 0)
    {
        wait(NULL);
        //decrement active processes
        active_processes--;
    }
    //end program
    return 0;
}

// Show help message
void show_help()
{
	printf("Use: mandelMovie [options]\n");
	printf("Where options are:\n");
	printf("-p <processes>  The maximum number of processors utilized by the program (default=1)\n");
	printf("-t <threads>    The number of threads utilized by the program (default=1)\n");
    printf("-h              Show this help text.\n");
	printf("\nSome examples are:\n");
	printf("./mandelMovie -p 5\n");
    printf("./mandelMovie -p 8\n");
    printf("./mandelMovie\n");
}
