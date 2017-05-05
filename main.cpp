#include <stdio.h>
#include <time.h>
#include "E101.h"

int main(){
	init();
	while(true) {
		int sum = 0;
		int i;
		int w;
		take_picture();
		for(i = -160; i<160; i++) {
			w = get_pixel(i+160,120,3);
			if (w > 128) {
				w = 1;
			} else {
				w = 0;
			}
			sum += w*i;
		}
		printf("The sum is %d\n",sum); 
		sleep1 (0, 500000);
		
	}
	
return 0;}
