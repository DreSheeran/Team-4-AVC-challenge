#include <stdio.h>
#include <time.h>
#include "E101.h"
#include <limits.h>

int main(){
	init();
	while(true) {
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
		printf("The sum is %d\n",sum); 
		sleep1 (0, 500000);
		
	}
	
return 0;}
