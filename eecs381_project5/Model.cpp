#include "Model.h"
#include "Ship.h"
#include "Island.h"
#include "View.h"
#include "Sim_object.h"
#include "Ship_factory.h"
#include "Utility.h"
#include <algorithm>
#include <list>

using namespace std;

Model&
Model::get_Instance()
{
    static Model model;
    return model;
}

Model::Model() :
    time(0)
{
    // create initial set of islands and ships
    // and place them into the appropriate containers
    shared_ptr<Island>
        exxon(make_shared<Island>("Exxon",Point(10,10),1000,200)),
        shell(make_shared<Island>("Shell",Point(0,30),1000,200)),
        bermuda(make_shared<Island>("Bermuda",Point(20,20))),
        island(make_shared<Island>("Treasure_Island",Point(50,5),100,5));
    
    sim_object_map["Exxon"]           = island_map["Exxon"]   = exxon;
    sim_object_map["Shell"]           = island_map["Shell"]   = shell;
    sim_object_map["Bermuda"]         = island_map["Bermuda"] = bermuda;
    sim_object_map["Treasure_Island"] = island_map["Treasure_Island"] = island;
    
    sim_object_map["Ajax"]    =
    ship_map["Ajax"]          = create_ship("Ajax","Cruiser",Point(15,15));
    sim_object_map["Xerxes"]  =
    ship_map["Xerxes"]        = create_ship("Xerxes","Cruiser",Point(25,25));
    sim_object_map["Valdez"]  =
    ship_map["Valdez"]        = create_ship("Valdez","Tanker",Point(30,30));
}

Model::~Model()
{

    // clear the maps
    island_map.clear();
    ship_map.clear();
    sim_object_map.clear();
}

bool
Model::is_name_in_use(const string& name) const
{
    // identical name in map
    if (sim_object_map.find(name) != sim_object_map.end())
    {
        return true;
    }

    // identical in first two characters
    return any_of(sim_object_map.begin(),
                  sim_object_map.end(),
                  [&name](pair<string, shared_ptr<Sim_object>> obj)
                  {return (obj.first[0] == name[0] &&
                           obj.first[1] == name[1]);});
}

bool
Model::is_island_present(const string& name) const
{
    return (island_map.find(name) != island_map.end());
}

void
Model::add_island(shared_ptr<Island> island_ptr)
{
    sim_object_map[island_ptr->get_name()] =
    island_map[island_ptr->get_name()]     = island_ptr;
    island_ptr->broadcast_current_state();
}

shared_ptr<Island>
Model::get_island_ptr(const string& name) const
{
    map<string, shared_ptr<Island>>::const_iterator it = island_map.find(name);
    if (it == island_map.end())
    {
        throw Error("Island not found!");
    }
    return it->second;
}

shared_ptr<Island>
Model::is_location_island(Point location) const
{
    return find_if(island_map.begin(),
                   island_map.end(),
                   [&location](pair<string, shared_ptr<Island>> obj)
                   {return (obj.second->get_location() == location);})->second;
}

std::vector<Point>
Model::get_island_locations() const
{
    vector<Point> return_vector;
    
    for_each(island_map.begin(),
             island_map.end(),
             [&return_vector](pair<string, shared_ptr<Island>> obj)
             {return_vector.push_back(obj.second->get_location());});
    
    return return_vector;
}

bool
Model::is_ship_present(const string& name) const
{
    return (ship_map.find(name) != ship_map.end());
}

void
Model::add_ship(shared_ptr<Ship> ship_ptr)
{
    sim_object_map[ship_ptr->get_name()] =
    ship_map[ship_ptr->get_name()]       = ship_ptr;
    ship_ptr->broadcast_current_state();
}

shared_ptr<Ship>
Model::get_ship_ptr(const string& name) const
{
    map<string, shared_ptr<Ship>>::const_iterator it = ship_map.find(name);
    if (it == ship_map.end())
    {
        throw Error("Ship not found!");
    }
    return it->second;
}

void
Model::describe() const
{
    for_each(sim_object_map.begin(),
             sim_object_map.end(),
             [](pair<string, shared_ptr<Sim_object>> obj){obj.second->describe();});
}

// increment the time, and tell all objects to update themselves
// then update the view.
void
Model::update()
{
    // increment time
    time++;

    // update all Sim_objects
    for_each(sim_object_map.begin(),
             sim_object_map.end(),
             [](pair<string, shared_ptr<Sim_object>> obj){obj.second->update();});

    // find all ships that are sunk and remove
    list<shared_ptr<Ship>> delete_ship_list;
    for_each(ship_map.begin(),
             ship_map.end(),
             [&delete_ship_list](pair<string, shared_ptr<Ship>> obj)
             {
                 if (!obj.second->is_afloat())
                 {
                     delete_ship_list.push_back(obj.second);
                 }
             });
    for_each(delete_ship_list.begin(),
             delete_ship_list.end(),
             [this](shared_ptr<Ship> ship_ptr)
             {
                 ship_map.erase(ship_map.find(ship_ptr->get_name()));
                 sim_object_map.erase(sim_object_map.find(ship_ptr->get_name()));
             });
}

void
Model::attach(shared_ptr<View> view_ptr)
{
    // insert view into container of view pointers
    view_array.push_back(view_ptr);

    // call broadcast_current_state for all sim_objects
    for_each(sim_object_map.begin(),
             sim_object_map.end(),
             [](pair<string, shared_ptr<Sim_object>> obj)
             {obj.second->broadcast_current_state();});
}

void
Model::detach(shared_ptr<View> view_ptr)
{
    view_array.erase(find(view_array.begin(), view_array.end(), view_ptr));
}

void
Model::notify_location(const string& name, Point location)
{
    for_each(view_array.begin(),
             view_array.end(),
             [&name, &location](shared_ptr<View> view_ptr)
             {view_ptr->update_location(name, location);});
}

// notify the views that an object is now gone
void
Model::notify_gone(const string& name)
{
    for_each(view_array.begin(),
             view_array.end(),
             [&name](shared_ptr<View> view_ptr)
             {view_ptr->update_remove(name);});
}
