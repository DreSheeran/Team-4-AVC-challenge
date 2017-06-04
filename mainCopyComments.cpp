#include <stdio.h> 
#include <time.h> 
#include "E101.h" 
#include <limits.h>

/*
 read_analog(0) = right
 read_analog(1) = front
 read_analog(7) = left
 motor 1 - right wheel
 motor 2 - left wheel
*/


bool allWhite = false;
int baseSpeed = 50;
int maxWhite = -1;
double multiplier = 1.2;

int lineCheck(int line) {
	int sum = 0;
	int i;
	int w;
	int numWhite = 0;
	int whiteThresh = 125;
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
	if(numWhite > maxWhite){
		maxWhite = numWhite;
		printf("Max whites seen is %d\n",maxWhite);
		
	}
	if (numWhite > 280) {
		allWhite = true;
	}

	return sum;
}

int wallCheck(int type) {
	int max = 0;
	for(int i = 0; i < 10; i++) {
		int cameraReading = read_analog(type);
		max += cameraReading;
	}
	int average = max/10;
	return average;
}

bool checkRed() {
	int line = 120;
	int red;
	int redThresh = 150;
	int otherThresh = 120;
	int numRed = 0;
	int green;
	int blue;
	take_picture();
	for(int i = 0; i<320; i++) {
		red = get_pixel(line,i,0);
		green = get_pixel(line,i,1);
		blue = get_pixel(line,i,2);
		if (red > redThresh && green < otherThresh && blue < otherThresh) {
			numRed++;
		}
	}
	if(numRed > 30) {
		return true;
	} else {
		return false;
	}
}

void runMotors2(int error) {
	int dimen = 600;
	if (error != INT_MAX) {
		set_motor(1,baseSpeed - error/dimen);
		set_motor(2,-baseSpeed - error/dimen);
	}
}

void runMotors3(int error) {
	int dimen = 23;
	set_motor(1,baseSpeed + error/dimen);
	set_motor(2,-baseSpeed + error/dimen);
}


void runMotors(int error) {
	int dimen = 450;
	if (error != INT_MAX) {
		set_motor(1,baseSpeed - error/dimen);
		set_motor(2,-baseSpeed - error/dimen);
	} else {
		set_motor(1,-baseSpeed);
		set_motor(2,baseSpeed);
		sleep1(0, 100000);
	}
}

void turnRight() {
	int error = INT_MAX;
	int topError = INT_MAX;
	int botError = INT_MAX;
	while(error == INT_MAX && topError == INT_MAX && botError == INT_MAX){
		set_motor(1,-baseSpeed);
		set_motor(2,-baseSpeed);
		error = lineCheck(120);
		topError = lineCheck(1);
		botError = lineCheck(239);
	}
}

void turnLeft() {
	int error = INT_MAX;
	int topError = INT_MAX;
	int botError = INT_MAX;
	while(error == INT_MAX && topError == INT_MAX && botError == INT_MAX) {
		set_motor(1,baseSpeed);
		set_motor(2,baseSpeed);
		error = lineCheck(120);
		topError = lineCheck(1);
		botError = lineCheck(239);
	}
}

void turnRight2() {
	int frontReading = INT_MAX;
	while(frontReading > 200) {
		set_motor(1,-baseSpeed*multiplier);
		set_motor(2,-baseSpeed*multiplier);
		frontReading = wallCheck(1);
	}

}

void turnLeft2() {
	int frontReading = INT_MAX;
	while(frontReading > 200) {
		set_motor(1,baseSpeed*multiplier);
		set_motor(2,baseSpeed*multiplier);
		frontReading = wallCheck(1);
	}
	
}


void openGate() {
	char server[15] = {0};
	sprintf(server, "130.195.6.196");
	connect_to_server(server, 1024);
	char message[24] ={0};
	sprintf(message, "Please");
	send_to_server(message);
	receive_from_server(message);
	send_to_server(message);
}

int main(){
	init();
	int error = 0;
	int topError = 0;
	int botError = 0;
	int lastError = 0;
	openGate();
	while(true) {
		error = lineCheck(120);
		runMotors(error);
		sleep1 (0, 100000);
		if (allWhite) {
			break;
		}
	}
	while(true) {
		error = lineCheck(120);
		topError = lineCheck(1);
		botError = lineCheck(239);
		if(topError == INT_MAX && botError == INT_MAX) {
			if(lastError > 200) {
				printf("Right turn %d\n",lastError);
				turnRight();
			} else {
				printf("Left turn %d\n",lastError);
				turnLeft();
			}
		} else {
			runMotors2(error);
		}
		if (error != INT_MAX) {
			lastError = error;
		}
		sleep1(0,10000);
		if(checkRed()) {
			break;
		}
	}
	while(true) {
		int frontError = wallCheck(1);
		int rightError = wallCheck(0);
		int leftError = wallCheck(7);
		if(checkRed() && frontError > 300) {
			set_motor(1,2);
			set_motor(2,2);
			continue;
		}
		if(frontError > 300) {
			if(rightError < 250) {
				turnRight2();
			} else if(leftError < 250) {
				turnLeft2();
			}
		} else {
			if(leftError < 100) {
				leftError = rightError;
			} else if(rightError < 100) {
				rightError = leftError;
			}
			int error = rightError - leftError;
			runMotors3(error);
		}
	}
	set_motor(1,2);
	set_motor(2,2);
	sleep1(1,0);
	return 0;
}
