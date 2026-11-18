#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/wait.h>

#define NUM_IMAGES 50

void show_help();

int main(int argc, char *argv[])
{
    char c;
    pid_t pid;

    int max_processes = 1;
    int active_processes = 0;
    double initial_scale = 4.0;
    double final_scale = 0.0005;

    while ((c = getopt(argc, argv, "p:h")) != -1)
    {
        switch (c)
        {
            case 'p':
                max_processes = atoi(optarg);
                break;
            case 'h':
                show_help();
                exit(0);
            case '?':
                show_help();
                exit(1);
        }
    }

    if (max_processes < 1)
        max_processes = 1;

    for (int i = 0; i < NUM_IMAGES; i++)
    {
        if (active_processes >= max_processes)
        {
            wait(NULL);
            active_processes--;
        }

        pid = fork();
        if (pid < 0)
        {
            perror("fork");
            exit(1);
        }
        if (pid == 0)
        {
            char scale_str[32], file_str[32];
            double scale = initial_scale *
                pow(final_scale / initial_scale, (double)i / (NUM_IMAGES - 1));

            snprintf(scale_str, sizeof(scale_str), "%.10f", scale);
            snprintf(file_str, sizeof(file_str), "mandel%d.jpg", i + 1);

            char *args[] = {"./mandel", "-x", "-0.38", "-y", "-0.665",
                            "-s", scale_str, "-m", "1000", "-o", file_str, NULL};
            execv("./mandel", args);
            perror("execv");
            exit(1);
        }
        active_processes++;
    }

    while (active_processes > 0)
    {
        wait(NULL);
        active_processes--;
    }
    return 0;
}

// Show help message
void show_help()
{
	printf("Use: mandelMovie [options]\n");
	printf("Where options are:\n");
	printf("-p <processes>  The maximum number of processors utilized by the program (default=1)\n");
	printf("-h          Show this help text.\n");
	printf("\nSome examples are:\n");
	printf("./mandelMovie -p 5\n");
    printf("./mandelMovie -p 8\n");
    printf("./mandelMovie\n");
}
