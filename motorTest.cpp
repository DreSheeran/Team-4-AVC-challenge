#include <stdio.h>
#include <time.h>
#include "E101.h"
#include <limits.h>

int main(){
  init();
	set_motor(1,-150);
  set_motor(2,150);
  sleep1(5,0);
  set_motor(1,2);
  set_motor(2,2);
  sleep1(1,0);
	
	
return 0;}
