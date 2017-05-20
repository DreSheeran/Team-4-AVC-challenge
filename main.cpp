#include <stdio.h>
#include <time.h>
#include "E101.h"
#include <limits.h>

//motor 1 - right wheel
//motor 2 - left wheel

bool quad3 = false;

int lineCheck(int line) {
	int sum = 0;
	int i;
	int w;
	int numWhite = 0;
	int whiteThresh = 100;
	take_picture();
	for(i = 0; i<320; i++) {
		w = get_pixel(line,i,3);
		if (w > whiteThresh) {
			w = 1;
			numWhite++;
		} else {
			w = 0;
		}
		sum += w*(i-160);
		
	}
	if(numWhite == 0){
		sum = INT_MAX;
	}
	if (numWhite == 320) {
		quad3 =true;
	}
	
	return sum;
}

void runMotors(int error) {
	int baseSpeed = 50;
	int dimen = 12500/30;
	if (error != INT_MAX) {
		set_motor(1,baseSpeed - error/dimen);
		set_motor(2,-baseSpeed - error/dimen);
	} else {
		set_motor(1,-baseSpeed);
		set_motor(2,baseSpeed);
		sleep1(0, 500000);
	}
}

int main(){
	init();
	int error = 0;
	int topError = 0;
	while(true) {
		error = lineCheck(120); 
		runMotors(error);
		sleep1 (0, 100000);	
		if (quad3) {
			break;
		}
	}
	
	while(true) {
		error = lineCheck(120); 
		topError = lineCheck(1); 
		if(topError == INT_MAX && error != INT_MAX) {
			set_motor(1,2);
			set_motor(2,2);
			break;
		}
		runMotors(error);
		sleep1 (0, 100000);	
	}
	
	return 0;
}


