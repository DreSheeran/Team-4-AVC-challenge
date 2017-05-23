#include <stdio.h> 
#include <time.h> 
#include "E101.h" 
#include <limits.h>

NICE CODE XDDDDD

Remember to log out!

//motor 1 - right wheel
//motor 2 - left wheel

bool allWhite = false;
int baseSpeed = 50;

int lineCheck(int line) {
	int sum = 0;
	int i;
	int w;
	int numWhite = 0;
	int whiteThresh = 120;
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
		allWhite = true;
	}

	return sum;
}

void runMotors(int error) {
	int dimen = 450;
	if (error != INT_MAX) {
		set_motor(1,baseSpeed - error/dimen);
		set_motor(2,-baseSpeed - error/dimen);
	} else {
		set_motor(1,-baseSpeed);
		set_motor(2,baseSpeed);
		sleep1(0, 300000);
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
			if(lastError > 8000) {
				turnRight();
			} else {
				turnLeft();
			}
		} else {
			runMotors(error);
		}
		sleep1 (0, 100000);
	}
	return 0;
}
