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
	int dimen = 250;
	if (error != INT_MAX) {
		set_motor(1,-150 + error/dimen);
		set_motor(2,150 + error/dimen);
	} else {
		set_motor(1,1);
		set_motor(2,1);
	}
}

int main(){
	init();
	int error = 0;
	while(true) {
		error = lineCheck(); 
		printf("The error is %d\n",error);
		runMotors(error);
		sleep1 (0, 500000);	
	}
	return 0;
}


