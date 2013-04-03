#ifndef ISLAND_H
#define ISLAND_H

/********************************************************************
    Islands are a kind of Sim_object;
    they have an amount of fuel and an amount by which it increases
    every update (default is zero).
    The can also provide or accept fuel, and update their amount
    accordingly.
********************************************************************/

#include "Sim_object.h"

class Island : public Sim_object
{
  public:
      // initialize then output constructor message
      Island (const std::string& name,
              Point position_,
              double fuel_ = 0.,
              double production_rate_ = 0.);

      // output destructor message
      ~Island();

      // forbid copy/move, construction/assignment
      Island(const Island&) = delete;
      Island(Island&&) = delete;
      Island& operator= (const Island&) = delete;
      Island& operator= (Island&&) = delete;

      // Return whichever is less, the request or the amount left,
      // update the amount on hand accordingly,
      // and output the amount supplied.
      double provide_fuel(double);

      // Add the amount to the amount on hand,
      // and output the total as the amount the Island now has.
      void accept_fuel(double);

      Point get_location() const override
          {return position;}

      // if production_rate > 0, compute production_rate * unit time,
      // and add to amount, and print an update message
      void update() override;

      // output information about the current state
      void describe() const override;

      // ask model to notify views of current state
      void broadcast_current_state() override;

  private:
      Point position;
      double fuel;
      double production_rate;
};

#endif

