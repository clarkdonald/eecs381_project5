#include "Controller.h"
#include "Model.h"
#include "Geometry.h"
#include "Views.h"
#include "Ship.h"
#include "Island.h"
#include "Ship_factory.h"
#include "Utility.h"
#include <algorithm>

using namespace std;

typedef map<string, void (Controller::*)()> No_Arg_Map_t;
typedef map<string, void (Controller::*)(shared_ptr<Ship>)> Ship_Arg_Map_t;
typedef map<string, void (Controller::*)(shared_ptr<View>)> View_Arg_Map_t;

Controller::Controller()
{
    // populate the functions needing a ship pointer arg
    ship_command_map["course"] = &Controller::ship_course;
    ship_command_map["position"] = &Controller::ship_position;
    ship_command_map["destination"] = &Controller::ship_destination;
    ship_command_map["load_at"] = &Controller::ship_load_at;
    ship_command_map["unload_at"] = &Controller::ship_unload_at;
    ship_command_map["dock_at"] = &Controller::ship_dock_at;
    ship_command_map["attack"] = &Controller::ship_attack;
    ship_command_map["refuel"] = &Controller::ship_refuel;
    ship_command_map["stop"] = &Controller::ship_stop;
    ship_command_map["stop_attack"] = &Controller::ship_stop_attack;
    
    // populate the functions for map
    map_command_map["default"] = &Controller::view_default;
    map_command_map["size"] = &Controller::view_size;
    map_command_map["zoom"] = &Controller::view_zoom;
    map_command_map["pan"] = &Controller::view_pan;
    map_command_map["open_map_view"] = &Controller::open_map_view;
    map_command_map["close_map_view"] = &Controller::close_map_view;
    
    // populate the functions for sailing
    sailing_command_map["open_sailing_view"] = &Controller::open_sailing_view;
    sailing_command_map["close_sailing_view"] = &Controller::close_sailing_view;
    
    // populate the functions for bridge
    bridge_command_map["open_bridge_view"] = &Controller::open_bridge_view;
    bridge_command_map["close_bridge_view"] = &Controller::close_bridge_view;

    // populate the functions needing no args
    no_arg_command_map["show"] = &Controller::view_show;
    no_arg_command_map["status"] = &Controller::model_status;
    no_arg_command_map["go"] = &Controller::model_go;
    no_arg_command_map["create"] = &Controller::model_create;
    
}

Controller::~Controller()
{
    ship_command_map.clear();
    no_arg_command_map.clear();
    map_command_map.clear();
    bridge_command_map.clear();
    sailing_command_map.clear();
}

// create View object,
// run the program by acccepting user commands,
// then destroy View object
void
Controller::run()
{
    shared_ptr<View> map_ptr(make_shared<Map_View>());
    shared_ptr<View> sailing_ptr(make_shared<Sailing_View>());
    
    // command loop to accept input from users
    while (true)
    {
        string first_word, second_word;
        No_Arg_Map_t::const_iterator no_arg_it;
        Ship_Arg_Map_t::const_iterator ship_arg_it;
        View_Arg_Map_t::const_iterator view_arg_it;

        cout << "\nTime " <<  Model::get_Instance().get_time()
             << ": Enter command: ";
        
        cin >> first_word;

        try
        {
            // quit
            if (first_word == "quit")
            {
                break;
            }
            // ship command
            else if (Model::get_Instance().is_ship_present(first_word))
            {
                cin >> second_word;
                if ((ship_arg_it = ship_command_map.find(second_word)) ==
                    ship_command_map.end())
                {
                    throw Error("Unrecognized command!");
                }
                (this->*(ship_arg_it->second))(Model::get_Instance().
                                                get_ship_ptr(first_word));
            }
            // map command
            else if ((view_arg_it = map_command_map.find(first_word)) !=
                     map_command_map.end())
            {
                (this->*(view_arg_it->second))(map_ptr);
            }
            // sailing command
            else if ((view_arg_it = sailing_command_map.find(first_word)) !=
                     sailing_command_map.end())
            {
                (this->*(view_arg_it->second))(sailing_ptr);
            }
            // bridge command
            else if ((ship_arg_it = bridge_command_map.find(first_word)) !=
                     bridge_command_map.end())
            {
                cin >> second_word;
                if (!Model::get_Instance().is_ship_present(second_word))
                {
                    throw Error("Ship not found!");
                }

                (this->*(ship_arg_it->second))(Model::get_Instance().get_ship_ptr(second_word));
            }
            // no arg command
            else if ((no_arg_it = no_arg_command_map.find(first_word)) !=
                     no_arg_command_map.end())
            {
                (this->*(no_arg_it->second))();
            }
            else
            {
                throw Error("Unrecognized command!");
            }
        }
        catch (Error& error)
        {
            cout << error.msg << endl;
            cin.clear();
            while (cin.get() != '\n');
        }
        catch (bad_alloc& error)
        {
            cout << "bad_alloc caught!" << endl;
            break;
        }
        catch (...)
        {
            cout << "Wow! Unknown exception caught!" << endl;
            break;
        }
    }

    cout << "Done" << endl;
}

// restore default settings of the map
void
Controller::view_default(shared_ptr<View> view_ptr)
{
    if (view_ptr == nullptr)
    {
        throw Error("Map view is not open!");
    }
    view_ptr->set_defaults();
}

// read a single integer for the size of the map
// integer is number of both rows and columns
void
Controller::view_size(shared_ptr<View> view_ptr)
{
    if (view_ptr == nullptr)
    {
        throw Error("Map view is not open!");
    }
    
    int size;
    if (!(cin >> size))
    {
        throw Error("Expected an integer!");
    }
    view_ptr->set_size(size);
}

// read a double value for the scale of the map
// double is number of nm per cell
void
Controller::view_zoom(shared_ptr<View> view_ptr)
{
    if (view_ptr == nullptr)
    {
        throw Error("Map view is not open!");
    }
    
    double scale;
    if (!(cin >> scale))
    {
        throw Error("Expected a double!");
    }
    view_ptr->set_scale(scale);
}

// read a pair of double value for the (x,y) origin of map
void
Controller::view_pan(shared_ptr<View> view_ptr)
{
    if (view_ptr == nullptr)
    {
        throw Error("Map view is not open!");
    }
    
    double x, y;
    if (!(cin >> x) || !(cin >> y))
    {
        throw Error("Expected a double!");
    }
    view_ptr->set_origin(Point(x,y));
}

// tell View to draw the map
void
Controller::view_show()
{
    for_each(view_container.begin(),
             view_container.end(),
             [](shared_ptr<View> view_ptr)
             {view_ptr->draw();});
}

// have all objects describe themselves
void
Controller::model_status()
{
    Model::get_Instance().describe();
}

// have all objects update
void
Controller::model_go()
{
    Model::get_Instance().update();
}

// read name for a new ship
// throw error if name is too short
// Model check for validity
// throw error if name is already in use
// read a type name for the new ship, and pair
// of doubles for initial position
// create new ship using Ship_factory and add to Model
void
Controller::model_create()
{
    double x, y;
    string name, type;

    cin >> name;
    if (name.length() < 2)
    {
        throw Error("Name is too short!");
    }
    if (Model::get_Instance().is_name_in_use(name))
    {
        throw Error("Name is already in use!");
    }

    cin >> type;
    if (!(cin >> x) || !(cin >> y))
    {
        throw Error("Expected a double!");
    }
    Model::get_Instance().add_ship(create_ship(name, type, Point(x,y)));
}

// read compass heading and speed (double)
// check: 0.0 <= compass heading < 360.0, speed >= 0.0
void
Controller::ship_course(shared_ptr<Ship>ship_ptr)
{
    double heading, speed;

    // error check heading
    if (!(cin >> heading))
    {
        throw Error("Expected a double!");
    }
    if (heading < 0.0 || heading >= 360.0)
    {
        throw Error("Invalid heading entered!");
    }
    speed = receive_and_check_speed();
    ship_ptr->set_course_and_speed(heading, speed);
}

// read (x,y) position then speed (double)
// check: x,y can have any value, speed >= 0.0
void
Controller::ship_position(shared_ptr<Ship>ship_ptr)
{
    double x, y, speed;

    if (!(cin >> x) || !(cin >> y))
    {
        throw Error("Expected a double!");
    }
    speed = receive_and_check_speed();
    ship_ptr->set_destination_position_and_speed(Point(x,y), speed);
}

// read Island name and speed
// position for Ship destination is position of Island
// check: Island must exist, speed >= 0.0
void
Controller::ship_destination(shared_ptr<Ship>ship_ptr)
{
    string name  = receive_and_check_island();
    double speed = receive_and_check_speed();
    ship_ptr->set_destination_position_and_speed(
        Model::get_Instance().get_island_ptr(name)->get_location(),
        speed);
}

// read Island name
// check: Island must exist
void
Controller::ship_load_at(shared_ptr<Ship>ship_ptr)
{
    string name = receive_and_check_island();
    ship_ptr->set_load_destination(Model::get_Instance().get_island_ptr(name));
}

// read Island name
// check: Island must exist
void
Controller::ship_unload_at(shared_ptr<Ship>ship_ptr)
{
    string name = receive_and_check_island();
    ship_ptr->set_unload_destination(Model::get_Instance().get_island_ptr(name));
}

// read Island name
// check: Island must exist
void
Controller::ship_dock_at(shared_ptr<Ship>ship_ptr)
{
    string name = receive_and_check_island();

    ship_ptr->dock(Model::get_Instance().get_island_ptr(name));
}

// read Ship name
// check: Ship must exist
void
Controller::ship_attack(shared_ptr<Ship>ship_ptr)
{
    string name = receive_and_check_ship();

    ship_ptr->attack(Model::get_Instance().get_ship_ptr(name));
}

void
Controller::ship_refuel(shared_ptr<Ship>ship_ptr)
{
    ship_ptr->refuel();
}

void
Controller::ship_stop(shared_ptr<Ship>ship_ptr)
{
    ship_ptr->stop();
}

void
Controller::ship_stop_attack(shared_ptr<Ship>ship_ptr)
{
    ship_ptr->stop_attack();
}

void Controller::open_map_view(std::shared_ptr<View> view_ptr)
{
    open_view(view_ptr, "Map view is already open!");
}

void
Controller::close_map_view(std::shared_ptr<View> view_ptr)
{
    close_view(view_ptr, "Map view is not open!");
}

void
Controller::open_sailing_view(std::shared_ptr<View> view_ptr)
{
    open_view(view_ptr, "Sailing data view is already open!");
}

void
Controller::close_sailing_view(std::shared_ptr<View> view_ptr)
{
    close_view(view_ptr, "Sailing data view is already open!");
}

void
Controller::open_bridge_view(std::shared_ptr<Ship> ship_ptr)
{
    auto it = bridge_map.find(ship_ptr->get_name());
    
    if (it != bridge_map.end())
    {
        throw Error("Bridge view is already open for that ship!");
    }
    
    shared_ptr<View> view_ptr(make_shared<Bridge_View>(ship_ptr->get_name(), ship_ptr->get_location(), ship_ptr->get_heading(), !ship_ptr->is_afloat()));
    
    Model::get_Instance().attach(view_ptr);
    bridge_map[ship_ptr->get_name()] = view_ptr;
    view_container.push_back(view_ptr);
}

void
Controller::close_bridge_view(shared_ptr<Ship> ship_ptr)
{
    auto it = bridge_map.find(ship_ptr->get_name());
    
    if (it == bridge_map.end())
    {
        throw Error("Bridge view for that ship is not open!");
    }
    
    Model::get_Instance().detach(it->second);
    bridge_map.erase(it);
    view_container.erase(find(view_container.begin(),
                              view_container.end(),
                              it->second));
}

double
Controller::receive_and_check_speed()
{
    double speed;
    if (!(cin >> speed))
    {
        throw Error("Expected a double!");
    }
    if (speed < 0.0)
    {
        throw Error("Negative speed entered!");
    }
    return speed;
}

string
Controller::receive_and_check_island()
{
    string name;
    cin >> name;
    if (!Model::get_Instance().is_island_present(name))
    {
        throw Error("Island not found!");
    }
    return name;
}

string
Controller::receive_and_check_ship()
{
    string name;
    cin >> name;
    if (!Model::get_Instance().is_ship_present(name))
    {
        throw Error("Ship not found!");
    }
    return name;
}

void
Controller::open_view(shared_ptr<View> view_ptr, const string& error)
{
    auto it = find(view_container.begin(),
                   view_container.end(),
                   view_ptr);
    
    if (it != view_container.end())
    {
        throw Error(error.c_str());
    }
    
    Model::get_Instance().attach(view_ptr);
    view_container.push_back(view_ptr);
}

void
Controller::close_view(shared_ptr<View> view_ptr, const string& error)
{
    auto it = find(view_container.begin(),
                   view_container.end(),
                   view_ptr);
    
    if (it == view_container.end())
    {
        throw Error(error.c_str());
    }
    
    Model::get_Instance().detach(view_ptr);
    view_container.erase(it);
}
