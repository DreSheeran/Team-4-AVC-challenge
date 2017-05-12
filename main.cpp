#include <stdio.h>
#include <time.h>
#include "E101.h"
#include <limits.h>



int lineCheck() {
	int sum = 0;
	int i;
	int w;
	int numWhite = 0;
	take_picture();
	for(i = 0; i<320; i++) {
		w = get_pixel(120,i,3);
		if (w > 80) {
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
	
	return sum;
}

void runMotors(int error) {
	int baseSpeed = 50;
	int dimen = 500;
	if (error != INT_MAX) {
		set_motor(1,baseSpeed - error/dimen);
		set_motor(2,-baseSpeed - error/dimen);
	} else {
		set_motor(1,-baseSpeed);
		set_motor(2,baseSpeed);
	}
}

int main(){
	init();
	int error = 0;
	while(true) {
		error = lineCheck(); 
		runMotors(error);
		sleep1 (0, 100000);	
	}
	return 0;
}


