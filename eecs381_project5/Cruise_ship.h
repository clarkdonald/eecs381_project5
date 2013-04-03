#ifndef CRUISE_SHIP_H
#define CRUISE_SHIP_H

#include "Ship.h"
#include <map>
#include <string>
#include <vector>

class Island;

class Cruise_ship : public Ship
{
  public:
      // constructor to initialize
      Cruise_ship(const std::string& name_, Point position_);
    
      // destructor
      ~Cruise_ship();
    
      // Update the state of the Cruise_ship
      void update() override;
    
      // output a description of current state to cout
      void describe() const override;
    
      // Start moving to a destination position at a speed
      // cancels any cruises if destination isn't an island
      // starts a cruise if the destination is an island
      virtual void set_destination_position_and_speed(Point, double) override;
    
      // Start moving on a course and speed
      // cancels any cruises
      virtual void set_course_and_speed(double, double) override;
    
      // Stop moving
      // cancels any cruises
      virtual void stop() override;
    
  private:
      double cruise_speed;
      std::shared_ptr<Island> first_island;
      std::shared_ptr<Island> next_island;
      std::vector<Point> visited_islands;
    
      enum Cruise_Ship_State_e
      {
          NOT_CRUISING,
          CRUISING,
          FIRST_UPDATE,
          SECOND_UPDATE,
          THIRD_UPDATE
      } cruise_ship_state;
    
      // stops the cruise by resetting all private values
      void stop_cruise();
    
      // obtain the next island to visit in cruise
      Point get_next_island();
};

#endif
