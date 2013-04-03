#ifndef MODEL_H
#define MODEL_H

/**************************************************************************
    Model is part of a simplified Model-View-Controller pattern.
    Model keeps track of the Sim_objects
    in our little world. It is the only
    component that knows how many Islands and
    Ships there are, but it does not
    know about any of their derived classes, 
    nor which Ships are of what kind of Ship. 
    It has facilities for looking up objects by name,
    and removing Ships.  When
    created, it creates an initial group of Islands
    and Ships using the Ship_factory.
    Finally, it keeps the system's time.

    Controller tells Model what to do; Model in turn
    tells the objects what do, and
    when asked to do so by an object, tells all
    the Views whenever anything changes that might be relevant.
    Model also provides facilities for looking up objects given their name.
***************************************************************************/

#include <vector>
#include <map>
#include <string>
#include <memory>

// incomplete forward declarations
struct Point;
class Sim_object;
class Island;
class Ship;
class View;

class Model
{
  public:
      // force Model as a singleton object
      static Model& get_Instance();

      // forbid copy/move, construction/assignment
      Model(const Model&) = delete;
      Model(Model&&) = delete;
      Model& operator= (const Model&) = delete;
      Model& operator= (Model&&) = delete;

      // return the current time
      int get_time()
          {return time;}

      // is name already in use for either ship or island?
      // either the identical name,
      // or identical in first two characters counts as in-use
      bool is_name_in_use(const std::string& name) const;

      // is there such an island?
      bool is_island_present(const std::string& name) const;

      // add a new island to the lists
      void add_island(std::shared_ptr<Island>);

      // will throw Error("Island not found!") if no island of that name
      std::shared_ptr<Island> get_island_ptr(const std::string& name) const;

      // is there such an ship?
      bool is_ship_present(const std::string& name) const;

      // add a new ship to the list, and update the view
      void add_ship(std::shared_ptr<Ship>);

      // will throw Error("Ship not found!") if no ship of that name
      std::shared_ptr<Ship> get_ship_ptr(const std::string& name) const;

      // tell all objects to describe themselves
      void describe() const;

      // increment the time, and tell
      // all objects to update themselves
      void update();    

      /* View services */
      // Attaching a View adds it to the
      // container and causes it to be updated
      // with all current objects'location
      // (or other state information.
      void attach(std::shared_ptr<View>);

      // Detach the View by discarding the
      // supplied pointer from the container of Views
      // - no updates sent to it thereafter.
      void detach(std::shared_ptr<View>);

      // notify the views about an object's location
      void notify_location(const std::string& name, Point location);

      // notify the views that an object is now gone
      void notify_gone(const std::string& name);

  private:
      // create the initial objects
      Model();
    
      // prevent accidental deletion
      ~Model();

      int time;
      std::map<std::string, std::shared_ptr<Sim_object>> sim_object_map;
      std::map<std::string, std::shared_ptr<Island>> island_map;
      std::map<std::string, std::shared_ptr<Ship>> ship_map;
      std::vector<std::shared_ptr<View>> view_array;
};

#endif
