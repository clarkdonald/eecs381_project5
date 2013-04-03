/*
Main module. Your project must use this file as-is
*/

#include "Controller.h"
#include <iostream>

using namespace std;

// The main function creates the Controller object, then tells it to run.

int main ()
{		
	// Set output to show two decimal places
	cout.setf(ios::fixed, ios::floatfield);
	cout.precision(2);

	// create the Controller and go
	Controller controller;

	controller.run();
}

