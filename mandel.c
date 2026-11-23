/// 
//  mandel.c
//  Based on example code found here:
//  https://users.cs.fiu.edu/~cpoellab/teaching/cop4610_fall22/project3.html
//
//  Converted to use jpg instead of BMP and other minor changes
//  
///
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "jpegrw.h"

// local routines
static int iteration_to_color( int i, int max );
static int iterations_at_point( double x, double y, int max );
static void compute_image(imgRawImage *img, double xmin, double xmax,
									double ymin, double ymax, int max, int max_threads);
static void show_help();
static void *compute_region(void *arg);

typedef struct {
    imgRawImage *img;
    double xmin, xmax, ymin, ymax;
    int max;
    int first_row;
    int last_row;
} thread_args;

int main(int argc, char *argv[])
{
	char c;

	// These are the default configuration values used
	// if no command line arguments are given.
	const char *outfile = "mandel.jpg";
	double xcenter = 0;
	double ycenter = 0;
	double xscale = 4;
	double yscale = 0; // calc later
	int    image_width = 1000;
	int    image_height = 1000;
	int    max = 1000;
	int max_threads = 1;

	// For each command line argument given,
	// override the appropriate configuration value.

	while((c = getopt(argc,argv,"t:x:y:s:W:H:m:o:h"))!=-1) {
		switch(c) 
		{
			case 'x':
				xcenter = atof(optarg);
				break;
			case 'y':
				ycenter = atof(optarg);
				break;
			case 's':
				xscale = atof(optarg);
				break;
			case 'W':
				image_width = atoi(optarg);
				break;
			case 'H':
				image_height = atoi(optarg);
				break;
			case 'm':
				max = atoi(optarg);
				break;
			case 'o':
				outfile = optarg;
				break;
			case 't':
                max_threads = atoi(optarg);
                break;
			case 'h':
				show_help();
				exit(1);
				break;
		}
	}
	// Calculate y scale based on x scale (settable) and image sizes in X and Y (settable)
	yscale = xscale / image_width * image_height;

	// Display the configuration of the image.
	printf("mandel: x=%lf y=%lf xscale=%lf yscale=%1f max=%d outfile=%s\n",xcenter,ycenter,xscale,yscale,max,outfile);

	// Create a raw image of the appropriate size.
	imgRawImage* img = initRawImage(image_width,image_height);

	// Fill it with a black
	setImageCOLOR(img,0);

	// Compute the Mandelbrot image
	compute_image(img,xcenter-xscale/2,xcenter+xscale/2,ycenter-yscale/2,ycenter+yscale/2,max,max_threads);

	// Save the image in the stated file.
	storeJpegImageFile(img,outfile);

	// free the mallocs
	freeRawImage(img);

	return 0;
}




/*
Return the number of iterations at point x, y
in the Mandelbrot space, up to a maximum of max.
*/

int iterations_at_point( double x, double y, int max )
{
	double x0 = x;
	double y0 = y;

	int iter = 0;

	while( (x*x + y*y <= 4) && iter < max ) {

		double xt = x*x - y*y + x0;
		double yt = 2*x*y + y0;

		x = xt;
		y = yt;

		iter++;
	}

	return iter;
}

/*
Compute an entire Mandelbrot image, writing each point to the given bitmap.
Scale the image to the range (xmin-xmax,ymin-ymax), limiting iterations to "max"
*/

void compute_image(imgRawImage* img, double xmin, double xmax, double ymin,
	double ymax, int max, int max_threads)
{
	pthread_t threads[20];
	thread_args args[20];

	int height = img->height;
	if (max_threads > height)
	{
		max_threads = height;
	}

	int rows = height / max_threads;
	int remainder = height % max_threads;
	int start = 0;
	int plus_one = 0;

	for (int t = 0; t < max_threads; t++)
	{
		if (t < remainder)
		{
			plus_one = 1;
		}
        int end = start + rows + plus_one;

        args[t] = (thread_args){img, xmin, xmax, ymin, ymax, max, start, end};

        if (pthread_create(&threads[t], NULL, compute_region, &args[t]) != 0)
		{
            perror("pthread_create");
            exit(1);
        }
        start = end;
		plus_one = 0;
    }

    for (int t = 0; t < max_threads; t++)
	{
        pthread_join(threads[t], NULL);
    }
}


/*
Convert a iteration number to a color.
Here, we just scale to gray with a maximum of imax.
Modify this function to make more interesting colors.
*/
int iteration_to_color( int iters, int max )
{
	int color = 0xFFFFFF*iters/(double)max;
	return color;
}


// Show help message
void show_help()
{
	printf("Use: mandel [options]\n");
	printf("Where options are:\n");
	printf("-t <threads>    The number of threads utilized by the program (default=1)\n");
	printf("-m <max>    The maximum number of iterations per point. (default=1000)\n");
	printf("-x <coord>  X coordinate of image center point. (default=0)\n");
	printf("-y <coord>  Y coordinate of image center point. (default=0)\n");
	printf("-s <scale>  Scale of the image in Mandlebrot coordinates (X-axis). (default=4)\n");
	printf("-W <pixels> Width of the image in pixels. (default=1000)\n");
	printf("-H <pixels> Height of the image in pixels. (default=1000)\n");
	printf("-o <file>   Set output file. (default=mandel.bmp)\n");
	printf("-h          Show this help text.\n");
	printf("\nSome examples are:\n");
	printf("mandel -x -0.5 -y -0.5 -s 0.2\n");
	printf("mandel -x -.38 -y -.665 -s .05 -m 100\n");
	printf("mandel -x 0.286932 -y 0.014287 -s .0005 -m 1000\n\n");
}

static void *compute_region(void *arg)
{
	thread_args *args = (thread_args *)arg;
	int width = args->img->width;
	int height = args->img->height;

	for (int j = args->first_row; j < args->last_row; j++)
	{
		for (int i = 0; i < width; i++)
		{
			double x = args->xmin + i * (args->xmax - args->xmin) / width;
            double y = args->ymin + j * (args->ymax - args->ymin) / height;
            int iters = iterations_at_point(x, y, args->max);
            setPixelCOLOR(args->img, i, j, iteration_to_color(iters, args->max));
        }
	}
	return NULL;
}
