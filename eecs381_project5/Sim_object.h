#ifndef SIM_OBJECT_H
#define SIM_OBJECT_H

/*************************************************
    This class provides the interface for all of
    simulation objects. It also stores the
    object's name, and has pure virtual accessor
    functions for the object's position
    and other information.
*************************************************/

#include <string>
#include <iostream>
#include "Geometry.h"

class Sim_object
{
  public:
      Sim_object(const std::string& name_) : 
          name(name_)
          {std::cout << "Sim_object " << name << " constructed" << std::endl;}

      virtual ~Sim_object()
          {std::cout << "Sim_object " << name << " destructed" << std::endl;}

      const std::string& get_name() const
          {return name;}

      // ask model to notify views of current state
      virtual void broadcast_current_state() {}

      // pure virtual function for getting the location of an object
      virtual Point get_location() const = 0;
 
      // pure virtual function for describing an object
      virtual void describe() const = 0;

      // pure virtual function for updating an object
      virtual void update() = 0;
        
  private:
      std::string name;
};

#endif
