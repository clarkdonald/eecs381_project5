#ifndef SHIP_H
#define SHIP_H

/***************************************************************************
    Ship Class
    A Ship has a name, initial position, amount of fuel, and parameters 
    that govern its movement. It can be commanded to 
    move to either a position or follow a course, or stop,
    dock at or refuel at an Island. It consumes fuel 
    while moving, and becomes immobile
    if it runs out of fuel. It inherits the Sim_object 
    interface to the rest of the system,
    and the Track_base class provides the basic movement 
    functionality, with the unit of time
    corresponding to 1.0 for one "tick" - an hour of simulated time.

    The update function updates the position and/or state of the ship.
    The describe function outputs information about the ship state.
    Accessors make the ship state available 
    to either the public or to derived classes.
    The is a "fat interface" for the capabilities
    of derived types of Ships. These
    functions are implemented in this class to throw an Error exception.
***************************************************************************/

#include "Sim_object.h"
#include "Track_base.h"
#include <string>

// forward declaration
class Island;

class Ship : public Sim_object
{
  public:
      // initialize, then output constructor message
      Ship(const std::string& name_,
           Point position_,
           double fuel_capacity_, 
           double maximum_speed_,
           double fuel_consumption_,
           int resistance_);

      // made pure virtual to mark this class as abstract
      // to output destructor message
      virtual ~Ship() = 0;

      // forbid copy/move, construction/assignment
      Ship(const Ship&) = delete;
      Ship(Ship&&) = delete;
      Ship& operator= (const Ship&) = delete;
      Ship& operator= (Ship&&) = delete;

      // return the current position
      Point get_location() const override
          {return track_base.get_position();}

      // Return true if ship can move
      // (it is not dead in the water or in the process or sinking); 
      bool can_move() const;

      // Return true if ship is moving; 
      bool is_moving() const;

      // Return true if ship is docked; 
      bool is_docked() const;

      // Return true if ship is afloat
      // (not in process of sinking), false if not
      bool is_afloat() const;

      // Return true if ship is on the bottom
      bool is_on_the_bottom() const;

      // Return true if Stopped and within 0.1 nm of the island
      bool can_dock(Island*) const;

      // Update the state of the Ship
      void update() override;

      // output a description of current state to cout
      void describe() const override;

      void broadcast_current_state() override;

      // Start moving to a destination position at a speed
      // may throw Error("Ship cannot move!")
      // may throw Error("Ship cannot go that fast!")
      virtual void set_destination_position_and_speed(Point, double);

      // Start moving on a course and speed
      // may throw Error("Ship cannot move!")
      // may throw Error("Ship cannot go that fast!");
      virtual void set_course_and_speed(double, double);

      // Stop moving
      // may throw Error("Ship cannot move!");
      virtual void stop();

      // dock at an Island -
      // set our position = Island's position, go into Docked state
      // may throw Error("Can't dock!");
      virtual void dock(Island *);

      // Refuel - must already be docked at an island;
      // fill takes as much as possible
      // may throw Error("Must be docked!");
      virtual void refuel();

      // These functions throw an Error exception for this class
      // will always throw Error("Cannot load at a destination!");
      virtual void set_load_destination(Island *);

      // will always throw Error("Cannot unload at a destination!");
      virtual void set_unload_destination(Island *);

      // will always throw Error("Cannot attack!");
      virtual void attack(std::shared_ptr<Ship>);

      // will always throw Error("Cannot attack!");
      virtual void stop_attack();

      // interactions with other objects
      // receive a hit from an attacker
      virtual void receive_hit(int, std::shared_ptr<Ship>);

  protected:
      double get_maximum_speed() const
          {return maximum_speed;}

      // return pointer to the Island currently docked at,
      // or nullptr if not docked
      Island* get_docked_Island() const
          {return docked_island;}

  private:
      Track_base track_base;
      double fuel;
      double fuel_capacity;
      double maximum_speed;
      double fuel_consumption;
      int resistance;
      Point destination;
      Island *docked_island;

      enum Ship_State_e
      {
          DOCKED,
          STOPPED,
          MOVING_TO_POSITION,
          MOVING_ON_COURSE,
          DEAD_IN_THE_WATER,
          SINKING,
          SUNK,
          ON_THE_BOTTOM
      } ship_state;

      // Updates position, fuel, and movement_state,
      // assuming 1 time unit (1 hr)
      void calculate_movement();

      // makes a check on the following:
      // 1) can ship move?
      // 2) is ship's speed fast enough?
      // throws exception for above being false
      void check_speed_and_move(double);
};

#endif
