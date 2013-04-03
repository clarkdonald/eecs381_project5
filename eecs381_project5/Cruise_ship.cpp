#include "Cruise_ship.h"
#include "Model.h"
#include "Island.h"
#include "Utility.h"

using namespace std;

Cruise_ship::Cruise_ship(const string& name_, Point position_) :
    Ship(name_, position_, 500., 15., 2., 0),
    cruise_speed(0.),
    first_island(nullptr),
    next_island(nullptr),
    cruise_ship_state(NOT_CRUISING)
{}

Cruise_ship::~Cruise_ship()
{}

void
Cruise_ship::update()
{
    Ship::update();
    
    if (cruise_ship_state != NOT_CRUISING)
    {
        if (!can_move() && cruise_ship_state == CRUISING)
        {
            stop_cruise();
        }
        else if (is_docked())
        {
            switch (cruise_ship_state)
            {
                case FIRST_UPDATE:
                    refuel();
                    cruise_ship_state = SECOND_UPDATE;
                    break;
                case SECOND_UPDATE:
                    cruise_ship_state = THIRD_UPDATE;
                    break;
                case THIRD_UPDATE:
                    Ship::set_destination_position_and_speed(get_next_island(), cruise_speed);
                    cruise_ship_state = CRUISING;
                    break;
                default:
                    break;
            }
        }
        else if (!is_moving() && can_dock(next_island) &&
                 cruise_ship_state == CRUISING)
        {
            dock(next_island);
            cruise_ship_state = FIRST_UPDATE;
        }
    }
}

void
Cruise_ship::describe() const
{
    cout << "\nCruise_ship ";
    Ship::describe();
    
    if (is_docked() && cruise_ship_state != NOT_CRUISING)
    {
        switch (cruise_ship_state)
        {
            case CRUISING:
                cout << "On cruise to "
                     << next_island->get_name()<< endl;
                break;
            case FIRST_UPDATE:
            case SECOND_UPDATE:
            case THIRD_UPDATE:
                cout << "Waiting during cruise at "
                     << get_docked_Island()->get_name() << endl;
                break;
            default:
                break;
        }
    }
}

void
Cruise_ship::set_destination_position_and_speed(Point destination, double speed)
{
    shared_ptr<Island> island_ptr;
    
    // if cruising, then we need to stop cruise
    stop_cruise();
    
    // ship is starting a cruise towards an island
    if ((island_ptr = Model::get_Instance().is_location_island(destination)))
    {
        cruise_speed      = speed;
        first_island      = island_ptr;
        next_island       = island_ptr;
        cruise_ship_state = CRUISING;
        visited_islands.push_back(island_ptr->get_location());
        cout << get_name() << " will visit "
            << island_ptr->get_name() << endl;
        cout << get_name() << " cruise will start and end at "
             << island_ptr->get_name() << endl;
    }
    
    Ship::set_destination_position_and_speed(destination, speed);
}

void
Cruise_ship::set_course_and_speed(double course, double speed)
{
    stop_cruise();
    Ship::set_course_and_speed(course, speed);
}

void
Cruise_ship::stop()
{
    stop_cruise();
    Ship::stop();
}

void
Cruise_ship::stop_cruise()
{
    if (cruise_ship_state != NOT_CRUISING)
    {
        cruise_speed      = 0.;
        first_island      = nullptr;
        next_island       = nullptr;
        cruise_ship_state = NOT_CRUISING;
        visited_islands.clear();
        cout << get_name() << " canceling current cruise" << endl;
    }
}

Point
Cruise_ship::get_next_island()
{
    
}