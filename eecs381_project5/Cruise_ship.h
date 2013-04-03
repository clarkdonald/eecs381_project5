#ifndef CRUISE_SHIP_H
#define CRUISE_SHIP_H

#include "Ship.h"
#include <string>

class Cruise_ship : public Ship
{
  public:
    // constructor to initialize
    Cruise_ship(const std::string& name_, Point position_);
    
    // destructor
    ~Cruise_ship();
    
    void update() override;
    
    void describe() const override;
    
  private:
    
};


#endif
