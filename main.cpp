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
		if (w > 128) {
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

int main(){
	init();
	int error = 0;
	while(true) {
		error = lineCheck(); 
		printf("The error is %d\n",error);
		sleep1 (0, 500000);	
	}
	return 0;
}


