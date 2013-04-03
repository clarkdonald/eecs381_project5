#include "Cruise_ship.h"
#include "Utility.h"

using namespace std;

Cruise_ship::Cruise_ship(const string& name_, Point position_) :
    Ship(name_, position_, 500., 15., 2., 0)
{}