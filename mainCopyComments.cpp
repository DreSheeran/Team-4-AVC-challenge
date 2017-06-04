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


bool allWhite = false; //becomes true to show the start of Quad 3
int baseSpeed = 50; //The base speed of the motors, assuming the line is perfectly in the center
int maxWhite = -1; //Will become the highest amount of whites seen during quad 2 - there for seeing if adjustment needed for threshholds
double multiplier = 1.2; //To increase motor power in turning corners during quad 4

/**
* Takes a picture from the camera, and looks at the specified line.
* If the average of the three color values is above the 'white threshhold', it counts as white.
* Else, it counts as black.
* This happens for every pixel in the line
* If it sees enough white, it will turn 'allwhite' to true.
* Returns a value based on where the white part is in relation to the center of the camera
*/
int lineCheck(int line) {
	int sum = 0;
	int i;
	int w;
	int numWhite = 0;
	int whiteThresh = 125; //White threshhold, can be easily changed here to change in all places
	take_picture(); 
	for(i = 0; i<320; i++) { //For every pixel in the row - the picture has 320 columns
		w = get_pixel(line,i,3); //w becomes the average value of all 3 color values
		if (w > whiteThresh) {
			w = 1; //w is then changed to 1 if it counts as white (in hindsight, this should probably have been recorded in a new variably rather than reassigning the same one)
			numWhite++;
		} else {
			w = 0; //If it is black, w becomes 0
		}
		sum += w*(i-160); //if w is 0, sum stays the same. If w is 1, sum is changed by an amount proportional to the distance from the center (if w is left of the center it decreases, to the right it increases)

	}
	if(numWhite == 0){
		sum = INT_MAX; //Sum becomes an impossibly high number to reach (2147483647), to show when no white can be seen
	}
	if(numWhite > maxWhite){ //Prints the number of whites seen if it is the highest so far - used for quick fixes during running the robot
		maxWhite = numWhite;
		printf("Max whites seen is %d\n",maxWhite); 
		
	}
	if (numWhite > 280) { //If enough whites seen it says all of them are white - used to start quad3 code
		allWhite = true;
	}

	return sum; //Returns the weighted sum worked out in the for loop
}

/**
* Similar to line check, but checks the walls in quad 4 instead.
* Will look at a wall determined by what number is passed in.
* read_analog(0) = right
* read_analog(1) = front
* read_analog(7) = left
* The larger the number returned, the closer the robot is to that side of the maze
*/
int wallCheck(int channel) {
	int max = 0;
	for(int i = 0; i < 10; i++) { // Takes the average of 10 readings, to reduce random noise
		int cameraReading = read_analog(channel);
		max += cameraReading;
	}
	int average = max/10;
	return average; 
}

/**
* Looks across the center line of the camera, and determines if the it is red.
* If it sees at least 30 rede pixels, it returns true, otherwise returns false
*/
bool checkRed() {
	int line = 120;
	int red;
	int redThresh = 150; //The red value must be above 150 for a pixel to be considered red
	int otherThresh = 120; //The other 2 values (green and blue) must be below 120 for the pixel to be considered red
	int numRed = 0;
	int green;
	int blue;
	take_picture();
	for(int i = 0; i<320; i++) {
		red = get_pixel(line,i,0); 
		green = get_pixel(line,i,1);
		blue = get_pixel(line,i,2);
		if (red > redThresh && green < otherThresh && blue < otherThresh) { //If the red is above the red threshhold and the green and blue are below the other threshhold, the pixel is red
			numRed++;
		}
	}
	if(numRed > 30) {
		return true;
	} else {
		return false;
	}
}

/**
* Code for the forward movement in the 3rd quadrant.
*/
void runMotors2(int error) {
	int dimen = 600;
	if (error != INT_MAX) {//As long as it can see some white
		set_motor(1,baseSpeed - error/dimen); //The motors get set based on the distance to the line
		set_motor(2,-baseSpeed - error/dimen);
	}
}

/**
* Code for the forward movement in the 4th quadrant - inside the maze
*/
void runMotors3(int error) {
	int dimen = 23;
	set_motor(1,baseSpeed + error/dimen); //The motors get set based on how close the bot is to the walls
	set_motor(2,-baseSpeed + error/dimen);
}

/**
* Code for the forward movement in the 1st and 2nd quadrants
*/
void runMotors(int error) {
	int dimen = 450;
	if (error != INT_MAX) { //As long as it can see some white
		set_motor(1,baseSpeed - error/dimen); //The motors get set based on the distance to the line
		set_motor(2,-baseSpeed - error/dimen);
	} else { //Will reverse straight back if it can only see black, and waits a little bit to make sure it finishes back on the line
		set_motor(1,-baseSpeed);
		set_motor(2,baseSpeed);
		sleep1(0, 100000);
	}
}

/**
* Turns the robot to the right in quad 3
* Will turn until it can see the white line somewhere in the camera
*/
void turnRight() {
	int error = INT_MAX;
	int topError = INT_MAX;
	int botError = INT_MAX;
	while(error == INT_MAX && topError == INT_MAX && botError == INT_MAX){ //While the top, middle and bottom lines can't see any white
		set_motor(1,-baseSpeed); //Turn right
		set_motor(2,-baseSpeed);
		error = lineCheck(120); 
		topError = lineCheck(1);
		botError = lineCheck(239);
	}
}

/**
* Turns the robot to the left in quad 3
* Will turn until it can see the white line somewhere in the camera
*/
void turnLeft() {
	int error = INT_MAX;
	int topError = INT_MAX;
	int botError = INT_MAX;
	while(error == INT_MAX && topError == INT_MAX && botError == INT_MAX) { //While the top, middle and bottom lines can't see any white
		set_motor(1,baseSpeed); //Turn left
		set_motor(2,baseSpeed);
		error = lineCheck(120);
		topError = lineCheck(1);
		botError = lineCheck(239);
	}
}

/**
* Turns the robot to the right in quad 4
* Will turn until it can't see a wall in front of it
*/
void turnRight2() {
	int frontReading = INT_MAX;
	while(frontReading > 200) { //While it can see a wall close enough in front of it
		set_motor(1,-baseSpeed*multiplier); //Turn right
		set_motor(2,-baseSpeed*multiplier);
		frontReading = wallCheck(1); //Check the reading to the front
	}

}

/**
* Turns the robot to the left in quad 4
* Will turn until it can't see a wall in front of it
*/
void turnLeft2() {
	int frontReading = INT_MAX;
	while(frontReading > 200) { //While it can see a wall close enough in front of it
		set_motor(1,baseSpeed*multiplier); //Turn left
		set_motor(2,baseSpeed*multiplier);
		frontReading = wallCheck(1); //Check the reading to the front
	}
	
}

/**
* Opens the gate at the start
* Connects to the server, send "Please" to it, recieves the message and sends it back
*/
void openGate() {
	char server[15] = {0}; //Creates a character array of 15 characters
	sprintf(server, "130.195.6.196"); //Puts the string for the IP into the array
	connect_to_server(server, 1024); //Connects to the server for the gate
	char message[24] ={0}; //Creates a character array for the message
	sprintf(message, "Please"); //Puts the string "Please" into the array
	send_to_server(message); //Sends the message array
	receive_from_server(message); //Recieve the message
	send_to_server(message); //Send it back
}

/**
* Code for the main control of the robot
* Each quadrant (1 and 2 in same one) in a seperate while loop from each other, and break out when condition met
*/
int main(){
	init();
	int error = 0;
	int topError = 0;
	int botError = 0;
	int lastError = 0;
	openGate(); //Opens the gate (pretty self-explanitory that one)
	while(true) { //Runs along the line for quadrants 1 and 2
		error = lineCheck(120); //Check the center line of the camera
		runMotors(error); //Run the motors foward based on position of line
		sleep1 (0, 100000); //Wait 0.1s
		if (allWhite) { //If it can see enough whites in the center line
			break; //Move on to quadrant 3 code
		}
	}
	while(true) { //Run through the tape maze in quadrant 3
		error = lineCheck(120);
		topError = lineCheck(1);
		botError = lineCheck(239);
		if(topError == INT_MAX && botError == INT_MAX) { //If both the top and bottom lines of the camera can't see any white
			if(lastError > 200) { //If the last reading for the center line showed it was far enough to the right of the robot, turn right until it sees the line again
				printf("Right turn %d\n",lastError);
				turnRight(); 
			} else { //Otherwise, turn left until it sees the line again
				printf("Left turn %d\n",lastError);
				turnLeft();
			}
		} else { //If it can see white, move forwards
			runMotors2(error);
		}
		if (error != INT_MAX) { //Sets the last error into the value seen in the center line
			lastError = error;
		}
		sleep1(0,10000); //Wait 0.01s
		if(checkRed()) { //If it can see red in the center, move onto quadrant 4 code
			break;
		}
	}
	while(true) { //Runs through the physical maze in quadrant 4 
		int frontError = wallCheck(1);
		int rightError = wallCheck(0);
		int leftError = wallCheck(7);
		if(checkRed() && frontError > 300) { //If it sees red and there is a wall in front of it
			set_motor(1,2); //Stop the motors
			set_motor(2,2);
			continue; //Go back to the start of the loop
		}
		if(frontError > 300) { //If it is close to the wall in front of it
			if(rightError < 250) { //If the gap to the right is large enough, turn rigt
				turnRight2();
			} else if(leftError < 250) { //Otherwise if the gap to the left is large enough, turn left
				turnLeft2();
			}
		} else { //If the wall in front is far enough away
			if(leftError < 100) { //If the left wall is very far away, pretend it is the same distance as the right wall
				leftError = rightError;
			} else if(rightError < 100) { //If the right wall is very far away, pretend it is the same distance as the left wall
				rightError = leftError;
			}
			int error = rightError - leftError; //See the position of the robot relative to the two walls - if it is positive the robot is closer to the right wall, negative it is closer to the left wall
			runMotors3(error); //Move forward based, turning to try stay an even distance between the walls
		}
	}
	set_motor(1,2); //pointless impossible to reach code(as the loop above is infinite) - just got left in by accident
	set_motor(2,2);
	sleep1(1,0);
	return 0;
}
