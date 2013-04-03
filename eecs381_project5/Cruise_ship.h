#ifndef CRUISE_SHIP_H
#define CRUISE_SHIP_H

#include "Ship.h"
#include <string>

class Cruise_ship : public Ship
{
  public:
    Cruise_ship(const std::string& name_, Point position_);
    
    ~Cruise_ship();
    
  private:
    
};


#endif
