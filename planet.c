#define _GNU_SOURCE
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <plot.h>
#include <float.h>
#include <unistd.h>

typedef struct planetStr {
	char name[420];
	char color[420];
	double size;
	double mass;
	double xPosition;
	double yPosition;
	double xVelocity;
	double yVelocity;
	double newVelocityX;
	double newVelocityY;
	double newPositionX;
	double newPositionY;
	double orbit;
	int year;
} planets;

planets* p;

void drawPlanets(plPlotter *plotter, int maxDistance) {
	/* get info on planets and draw them */
	pl_pencolorname_r(plotter,"white");
	for (int i = 0; i < 5; i++) {
		pl_fillcolorname_r(plotter, p[i].color);
		pl_fcircle_r(
			plotter, 
			p[i].xPosition, 
			p[i].yPosition, 
			p[i].size * 399999999999);
	}
}


int main(int argc, char** argv) {
	char buff[420];
	int pos, arrayPos = 0;
	double maxDistanceX, maxDistanceY = 0;
	FILE* fd = fopen("planet.dat", "r");
	fgets(buff, 420, fd);
	p = malloc(sizeof(planets) * 12);
	while(fgets(buff, 420, fd) != NULL) { //read file
		char* fluff = &buff[0];
		if (*fluff != '#') { //skip over comments
			double distanceX, distanceY = 0;
			sscanf(buff, "%s %s %lf %lf %lf %lf %lf %lf",
				p[arrayPos].name, 
				p[arrayPos].color, 
				&p[arrayPos].size, 
				&p[arrayPos].mass, 
				&p[arrayPos].xPosition, 
				&p[arrayPos].yPosition, 
				&p[arrayPos].xVelocity, 
				&p[arrayPos].yVelocity);	
			if (pos < 5) {
				distanceX = p[arrayPos].xPosition;			
				distanceY = p[arrayPos].yPosition;		
				arrayPos++;
				if (fabs(distanceX) > maxDistanceX) {
					maxDistanceX = fabs(distanceX);
				}
				if (fabs(distanceY) > maxDistanceY) {
					maxDistanceY = fabs(distanceY);
				}
			}
			pos++;
		}
	}

	double maxDistance = 0;
	if (maxDistanceX > maxDistanceY) {
		maxDistance = maxDistanceX * 1.3;
	} else {
		maxDistance = maxDistanceY * 1.3;
	}

	/* plotter device structures */
  	plPlotter *plotter;
  	plPlotterParams *plotterParams;

  	/* create a plotter parametric structure */
  	plotterParams = pl_newplparams();
  	pl_setplparam(plotterParams, "BITMAPSIZE", "750x750");
  	pl_setplparam(plotterParams, "USE_DOUBLE_BUFFERING", "no");
	pl_setplparam(plotterParams, "BG_COLOR", "black");

	/* create the plotter device and open it */
  	if ((plotter = pl_newpl_r("X", stdin, stdout, stderr, plotterParams)) == NULL) {
    		fprintf(stderr, "Couldn't create Xwindows plotter\n");
    		exit(1);
  	} else if (pl_openpl_r(plotter) < 0) {
    		fprintf(stderr, "Couldn't open Xwindows plotter\n");
    		exit(1);
  	}

	/* coordinate space */
	pl_fspace_r(plotter, -maxDistance, -maxDistance, maxDistance, maxDistance);

	/* pick a type for the pen and the fill */
	pl_pentype_r(plotter,1);
	pl_filltype_r(plotter,1);

	/**********Euler's Method**********/
	double dt = 300;
	double ft = 24.0 * 3600 * 10000;
	double G = 0.0000000000667259; //universal gravitation constant
	for (int a = 0; a < ft/dt ; a++){
		//updatePlanets(dt);
		//plotPlanets(dt);
		//if (a%100==0) plotPlanets;
		for (int j = 0; j < 5; j++) {
			double Ax = 0.0;
            		double Ay = 0.0;
			for (int i = 0; i < 5; i++) {
				if (j != i){
					double r = sqrt(pow((p[j].xPosition)-(p[i].xPosition),2)
						+ pow((p[j].yPosition)-(p[i].yPosition), 2));
					Ax += (p[i].mass * G * (p[i].xPosition - p[j].xPosition)) / pow(r, 3);
		            		Ay += (p[i].mass * G * (p[i].yPosition - p[j].yPosition)) / pow(r, 3);
				}
			}
			//calculate and store new velocities
			p[j].newVelocityX = p[j].xVelocity + dt*Ax;
            		p[j].newVelocityY = p[j].yVelocity + dt*Ay;
			//calculate and store new positions
            		p[j].newPositionX = p[j].xPosition + dt*p[j].xVelocity;
            		p[j].newPositionY = p[j].yPosition + dt*p[j].yVelocity;
		}
		for (int k = 0; k < 5; k++) {
			//set old velocity/positions to the new ones
			p[k].xVelocity = p[k].newVelocityX;
			p[k].yVelocity = p[k].newVelocityY;
			p[k].xPosition = p[k].newPositionX;
			p[k].yPosition = p[k].newPositionY;
		}
		if (a % 200 == 0) {
			pl_erase_r(plotter); //erase window
			//pl_pencolorname_r(plotter,"white");
			drawPlanets(plotter, maxDistance);
		}
	}
	/***********************************/
			
	/* close and cleanup the plotter stuff */
  	if (pl_closepl_r(plotter) < 0) {
    		fprintf(stderr, "Couldn't close plotter\n");
  	} else if (pl_deletepl_r(plotter) < 0) {
    		fprintf(stderr, "Couldn't delete plotter\n");
  	}
	return 0;
}
