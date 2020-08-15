/* Sample UDP server */
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

enum walk_speed {NORMAL, DAWDLING, SLOW, FAST, SPRINT};

/* NOTE: need to link math lib when compiling: gcc SF_UnionSquare_veryslow.c -lm */
char targetFile[256];
long double coordinate[1000][3];


int interval;
float elevation;
int iSSmallArea;
int locElevation;
int nMax;
int nMin = 1; 

void init()
{
	/** Variable "interval" is used tO control the walking speed.
	  	The higher the interval, the slower the person walks.
		Note: 1. originally it was set to 900, but the account got soft banned after 1 day with Gotcha:
		 	 catching 600+ pokemons, spinging 570 pokestops, and walking km. 
 	*/
	if (iSSmallArea)
	{
		interval = 100; 
		nMax = 100;
	}
	else
	{
		interval = 900; 
		nMax = 400;
	}
	if (locElevation)
		elevation = locElevation;
	else
		elevation = 50.0;
}


long double oneMile = 0.018279863; /* This is coordinate distance bwteen 2 points with ~1.6km apart */
long double smallUnit = 10.0; /* When smallUnit is 10.0, This is ~160m apart */

long double findSlope(long double x1, long double y1, long double x2, long double y2)
{
	long double slope = 0.0;

	if (x2 != x1)
		slope = (y2 - y1) / (x2 - x1);

	return slope;
}

long double findDistance(long double x1, long double y1, long double x2, long double y2)
{
	long double distance = 0.0;
	long double X_sqared = (x2 - x1)*(x2 - x1);
	long double y_squared = (y2 - y1)*(y2 - y1);

	distance = sqrtl(X_sqared) + sqrtl(y_squared);
	return distance;
}

int main(int argc, char **argv)
{
	int size = sizeof (coordinate) / sizeof(long double) / 3;
	int i = 0;
	int index = 0;
	long double unitDistance =  oneMile / smallUnit; /*This is ~160m apart */

	FILE *fptr;
	int randomNumber = 0;
	float randomElevation = 0.0;
	int total_wpt = 0;

	init();
	srand(time(NULL));

	printf("open taget file : %s\n", targetFile);
	fptr = fopen(targetFile, "w");

	if (fptr == NULL)
	{
		printf("Error!");
		return -1;
	}

	fprintf(fptr, "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>\n");
	fprintf(fptr, "<gpx version=\"1.1\" creator=\"Xcode\">\n");

	fprintf(fptr, "\t<!--\n");
	fprintf(fptr, "\t\tProvide one or more waypoints containing a latitude/longitude pair.\n");
	fprintf(fptr, "\t\tIf you provide one waypoint, Xcode will simulate that specific location.\n");
	fprintf(fptr, "\t\tIf you provide multiple waypoints, Xcode will simulate a route visiting each waypoint.\n");
	fprintf(fptr, "\t-->\n");

	fprintf(fptr, "\t<!--\n");
	fprintf(fptr, "\t\tOptionally provide a time element for each waypoint.\n");
	fprintf(fptr, "\t\tXcode will interpolate movement at a rate of speed based on the time elapsed\n");
	fprintf(fptr, "\t\tbetween each waypoint. If you do not provide a time element\n");
	fprintf(fptr, "\t\tthen Xcode will use a fixed rate of speed.\n");
	fprintf(fptr, "\t\tWaypoints must be sorted by time in ascending order.\n");
	fprintf(fptr, "\t\te.g., <time>2020-07-30T11:35:29Z</time>\n");
	fprintf(fptr, "\t-->\n");
	
	for (i = 0; i < size - 2; i++)
	{
		long double slope = 0.0;
		long double distance = 0.0;
		long double intercept = 0.0;
		long double x0 = coordinate[i][0];
		long double x_increase = 0.0;
		int j = 0;
		int counter = 0;
		int each_leg_interval = interval;
		int interval_per_wpt = interval;
		int nMax_per_wpt = nMax;

		if (coordinate[i+1][2] == DAWDLING)
		{
			interval_per_wpt = interval * 4;
			nMax_per_wpt = nMax * 2;
		}
		if (coordinate[i+1][2] == SLOW)
		{
			interval_per_wpt = interval * 2;
			nMax_per_wpt = nMax * 2;
		}
		else if (coordinate[i+1][2] == NORMAL)
		{
			interval_per_wpt = interval;
			nMax_per_wpt = nMax;
		}
		else if (coordinate[i+1][2] == FAST)
		{
			interval_per_wpt = interval / 4;
			nMax_per_wpt = nMax / 4;
		}
		 else if (coordinate[i+1][2] == SPRINT)
		{
			interval_per_wpt = interval / 16;
			nMax_per_wpt = nMax / 16;
		}


		if (coordinate[i+1][0] == 0.0 && coordinate[i+1][1] == 0.0)
		{
			break;
		}
		slope = findSlope (coordinate[i][0], coordinate[i][1], coordinate[i+1][0], coordinate[i+1][1]);
		intercept =  coordinate[i+1][1] - slope * coordinate[i+1][0];

#if 0
		printf ("slope = %Lf, x1=%Lf, x2=%Lf, y1=%Lf, y2=%Lf, intercept=%Lf\n",
			slope, coordinate[i][0], coordinate[i+1][0], coordinate[i][1], coordinate[i+1][1], intercept);
#endif

		distance = findDistance (coordinate[i][0], coordinate[i][1], coordinate[i+1][0], coordinate[i+1][1]);

		if (distance >= unitDistance)
		{
			randomNumber = 0;
			each_leg_interval= interval_per_wpt;
			randomElevation = 0;
		}
		else
		{
			randomNumber = rand()%(nMax_per_wpt - nMin) + nMin;
			each_leg_interval = interval_per_wpt + randomNumber;
			randomElevation = (rand() % 10000) / 10000.0;
		}

		counter = (int) (each_leg_interval * (long double)distance / (long double)unitDistance);
#if 0
		printf(" counter=%d, interval_per_wpt=%d, randomNumber=%d, each_leg_interval=%d, nMax_per_wpt=%d\n",
			counter, interval_per_wpt, randomNumber, each_leg_interval, nMax_per_wpt);
#endif
		if (counter <= 0 )
		{
			counter = 1;
		}

		total_wpt += counter;
		x_increase = (coordinate[i+1][0] - coordinate[i][0] ) / counter;
		for (j = 0; j <  counter; j++)
		{
			long double x = x0 + j * x_increase;
			long double y = 0.0;

			y = slope * x + intercept;
			fprintf(fptr, "	<wpt lat=\"%Lf\" lon=\"%Lf\">\n", x, y);
			fprintf(fptr, "		<name>WP%d</name>\n", index++);
			fprintf(fptr, "		<ele>%f</ele>\n", elevation+randomElevation);
			fprintf(fptr, "	</wpt>\n");
		}

	}
	fprintf(fptr, "</gpx>\n");
	fclose(fptr);
	//printf("\n===LMA total_wpt=%d\n", total_wpt);
	return 0;
}
