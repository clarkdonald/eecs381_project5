#include "Controller.h"
#include "Model.h"
#include "Geometry.h"
#include "View.h"
#include "Ship.h"
#include "Island.h"
#include "Ship_factory.h"
#include "Utility.h"

using namespace std;

Controller::Controller()
{
    // populate the ship command map
    ship_command_map["course"]      = &Controller::ship_course;
    ship_command_map["position"]    = &Controller::ship_position;
    ship_command_map["destination"] = &Controller::ship_destination;
    ship_command_map["load_at"]     = &Controller::ship_load_at;
    ship_command_map["unload_at"]   = &Controller::ship_unload_at;
    ship_command_map["dock_at"]     = &Controller::ship_dock_at;
    ship_command_map["attack"]      = &Controller::ship_attack;
    ship_command_map["refuel"]      = &Controller::ship_refuel;
    ship_command_map["stop"]        = &Controller::ship_stop;
    ship_command_map["stop_attack"] = &Controller::ship_stop_attack;

    // populate the view command map
    view_command_map["default"]     = &Controller::view_default;
    view_command_map["size"]        = &Controller::view_size;
    view_command_map["zoom"]        = &Controller::view_zoom;
    view_command_map["pan"]         = &Controller::view_pan;
    view_command_map["show"]        = &Controller::view_show;

    // populate the model command map
    model_command_map["status"]     = &Controller::model_status;
    model_command_map["go"]         = &Controller::model_go;
    model_command_map["create"]     = &Controller::model_create;

    cout << "Controller constructed" << endl;
}

Controller::~Controller()
{
    ship_command_map.clear();
    model_command_map.clear();
    view_command_map.clear();
    cout << "Controller destructed" << endl;
}

// create View object,
// run the program by acccepting user commands,
// then destroy View object
void
Controller::run()
{
    shared_ptr<View> view_ptr(new View);
    g_Model_ptr->attach(view_ptr);

    // command loop to accept input from users
    while (true)
    {
        string first_word, second_word;
        map<string, void (Controller::*)()>::const_iterator m_command_it;
        map<std::string, void (Controller::*)(shared_ptr<Ship>)>::const_iterator s_command_it;
        map<std::string, void (Controller::*)(shared_ptr<View>)>::const_iterator v_command_it;

        cout << "\nTime " <<  g_Model_ptr->get_time() << ": Enter command: ";
        cin >> first_word;

        try
        {
            // quit
            if (first_word == "quit")
            {
                break;
            }
            // ship command
            else if (g_Model_ptr->is_ship_present(first_word))
            {
                cin >> second_word;
                if ((s_command_it = ship_command_map.find(second_word)) ==
                    ship_command_map.end())
                {
                    throw Error("Unrecognized command!");
                }
                (this->*(s_command_it->second))(g_Model_ptr->get_ship_ptr(first_word));
            }
            // model command
            else if ((m_command_it = model_command_map.find(first_word)) !=
                     model_command_map.end())
            {
                (this->*(m_command_it->second))();
            }
            // view command
            else if ((v_command_it = view_command_map.find(first_word)) !=
                     view_command_map.end())
            {
                (this->*(v_command_it->second))(view_ptr);
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

    // final clean-up before exiting
    g_Model_ptr->detach(view_ptr);
    cout << "Done" << endl;
}

// restore default settings of the map
void
Controller::view_default(shared_ptr<View> view_ptr)
{
    view_ptr->set_defaults();
}

// read a single integer for the size of the map
// integer is number of both rows and columns
void
Controller::view_size(shared_ptr<View> view_ptr)
{
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
    double x, y;
    if (!(cin >> x) || !(cin >> y))
    {
        throw Error("Expected a double!");
    }
    view_ptr->set_origin(Point(x,y));
}

// tell View to draw the map
void
Controller::view_show(shared_ptr<View> view_ptr)
{
    view_ptr->draw();
}

// have all objects describe themselves
void
Controller::model_status()
{
    g_Model_ptr->describe();
}

// have all objects update
void
Controller::model_go()
{
    g_Model_ptr->update();
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
    if (g_Model_ptr->is_name_in_use(name))
    {
        throw Error("Name is already in use!");
    }

    cin >> type;
    if (!(cin >> x) || !(cin >> y))
    {
        throw Error("Expected a double!");
    }
    g_Model_ptr->add_ship(create_ship(name, type, Point(x,y)));
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
        g_Model_ptr->get_island_ptr(name)->get_location(),
        speed);
}

// read Island name
// check: Island must exist
void
Controller::ship_load_at(shared_ptr<Ship>ship_ptr)
{
    string name = receive_and_check_island();
    ship_ptr->set_load_destination(g_Model_ptr->get_island_ptr(name));
}

// read Island name
// check: Island must exist
void
Controller::ship_unload_at(shared_ptr<Ship>ship_ptr)
{
    string name = receive_and_check_island();
    ship_ptr->set_unload_destination(g_Model_ptr->get_island_ptr(name));
}

// read Island name
// check: Island must exist
void
Controller::ship_dock_at(shared_ptr<Ship>ship_ptr)
{
    string name = receive_and_check_island();

    ship_ptr->dock(g_Model_ptr->get_island_ptr(name));
}

// read Ship name
// check: Ship must exist
void
Controller::ship_attack(shared_ptr<Ship>ship_ptr)
{
    string name = receive_and_check_ship();

    ship_ptr->attack(g_Model_ptr->get_ship_ptr(name));
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
    if (!g_Model_ptr->is_island_present(name))
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
    if (!g_Model_ptr->is_ship_present(name))
    {
        throw Error("Ship not found!");
    }
    return name;
}
