#include "Ship.h"
#include "Model.h"
#include "Island.h"
#include "Utility.h"

using namespace std;

Ship::Ship(const string& name_,
           Point position_,
           double fuel_capacity_, 
           double maximum_speed_,
           double fuel_consumption_,
           int resistance_) :
    Sim_object(name_),
    track_base(position_),
    fuel(fuel_capacity_),
    fuel_capacity(fuel_capacity_),
    maximum_speed(maximum_speed_),
    fuel_consumption(fuel_consumption_),
    resistance(resistance_),
    docked_island(nullptr),
    ship_state(STOPPED)
{}

Ship::~Ship()
{}

bool
Ship::can_move() const
{
    if (!is_afloat() ||
        ship_state == DEAD_IN_THE_WATER)
    {
        return false;
    } 
    return true;
}

bool
Ship::is_moving() const
{
    if (ship_state == MOVING_TO_POSITION ||
        ship_state == MOVING_ON_COURSE)
    {
        return true;
    }
    return false;
}

bool
Ship::is_docked() const
{
    if (ship_state == DOCKED)
    {
        return true;
    }
    return false;
}

bool
Ship::is_afloat() const
{
    if (ship_state == SUNK)
    {
        return false;
    }
    return true;
}

bool
Ship::can_dock(shared_ptr<Island> island_ptr) const
{
    if (ship_state == STOPPED &&
        cartesian_distance(island_ptr->get_location(),
                           get_location()) <= 0.1)

    {
        return true;
    }
    return false;
}

// Update the state of the Ship
void
Ship::update()
{
    // ship is still afloat
    if (is_afloat())
    {
        if (resistance < 0)
        {
            ship_state = SUNK;
            cout << get_name() << " sunk" << endl;
            Model::get_Instance().notify_gone(get_name());
        }
        else
        {
            switch (ship_state)
            {
                case MOVING_TO_POSITION:
                case MOVING_ON_COURSE:
                    calculate_movement();
                    cout << get_name() << " now at "
                         << track_base.get_position() << endl;
                    Model::get_Instance().notify_location(get_name(),
                                                 track_base.get_position());
                    break;
                case DOCKED:
                    cout << get_name() << " docked at "
                         << get_docked_Island()->get_name() << endl;
                    break;
                case STOPPED:
                    cout << get_name() << " stopped at "
                         << track_base.get_position() << endl;
                    break;
                case DEAD_IN_THE_WATER:
                    cout << get_name() << " dead in the water at "
                         << track_base.get_position() << endl;
                    break;
                default:
                    break;
            }
        }
    }
}

void
Ship::describe() const
{
    // output ship's name and position
    cout << get_name() << " at " << track_base.get_position();

    // output for ship that is not afloat
    if (!is_afloat())
    {
        cout << " sunk" << endl;
    }
    // output for ship that is afloat
    else
    {
        cout << ", fuel: " << fuel << " tons"
             << ", resistance: " << resistance << endl;

        switch (ship_state)
        {
            case MOVING_TO_POSITION:
                cout << "Moving to " << destination
                     << " on " << track_base.get_course_speed() << endl;
                break;
            case MOVING_ON_COURSE:
              cout << "Moving on " << track_base.get_course_speed() << endl;
                break;
            case DOCKED:
                cout << "Docked at " << get_docked_Island()->get_name()
                     << endl;
                break;
            case STOPPED:
                cout << "Stopped" << endl;
                break;
            case DEAD_IN_THE_WATER:
                cout << "Dead in the water" << endl;
                break;
            default:
                break;
        }
    }
}

void
Ship::broadcast_current_state()
{
    Model::get_Instance().notify_location(get_name(), get_location());
}

void
Ship::set_destination_position_and_speed(Point destination_position,
                                         double speed)
{
    check_speed_and_move(speed);
    Compass_vector compass_vector(get_location(), destination_position);
    track_base.set_course(compass_vector.direction);
    track_base.set_speed(speed);
    destination = destination_position;
    ship_state  = MOVING_TO_POSITION;
    cout << get_name() << " will sail on "
         << track_base.get_course_speed() << " to " << destination << endl;
}

void
Ship::set_course_and_speed(double course, double speed)
{
    check_speed_and_move(speed);
    track_base.set_course(course);
    track_base.set_speed(speed);
    ship_state = MOVING_ON_COURSE;
    cout << get_name() << " will sail on "
         << track_base.get_course_speed() << endl;
}

void
Ship::stop()
{
    if (!can_move())
    {
        throw Error("Ship cannot move!");
    }
    track_base.set_speed(0.0);
    ship_state = STOPPED;
    cout << get_name() << " stopping at " << get_location() << endl;
}

void
Ship::dock(shared_ptr<Island> island_ptr)
{
    if (is_moving() || !can_dock(island_ptr))
    {
        throw Error("Can't dock!");
    }
    track_base.set_position(island_ptr->get_location());
    Model::get_Instance().notify_location(get_name(), get_location());
    docked_island = island_ptr;
    ship_state    = DOCKED;
    cout << get_name() << " docked at " << island_ptr->get_name() << endl;
}

void
Ship::refuel()
{
    if (!is_docked())
    {
        throw Error("Must be docked!");
    }
    double fuel_needed_to_fill = fuel_capacity - fuel;
    if (fuel_needed_to_fill < 0.005)
    {
        fuel = fuel_capacity;
    }
    else
    {
        fuel += get_docked_Island()->provide_fuel(fuel_needed_to_fill);
        cout << get_name() << " now has " << fuel << " tons of fuel" << endl;
    }
}

void
Ship::set_load_destination(shared_ptr<Island> island_ptr)
{
    throw Error("Cannot load at a destination!");
}

void
Ship::set_unload_destination(shared_ptr<Island> island_ptr)
{
    throw Error("Cannot unload at a destination!");
}

void
Ship::attack(shared_ptr<Ship> in_target_ptr)
{
    throw Error("Cannot attack!");
}

void
Ship::stop_attack()
{
    throw Error("Cannot attack!");
}

void
Ship::receive_hit(int hit_force, shared_ptr<Ship> attacker_ptr)
{
    resistance -= hit_force;
    cout << get_name() << " hit with "
         << hit_force << ", resistance now "
         << resistance << endl;
}

// Calculate the new position of a ship based on
// how it is moving, its speed, and
// fuel state. This function should be
// called only if the state is 
// MOVING_TO_POSITION or MOVING_ON_COURSE.
//
// Track_base has an update_position(double time)
// function that computes the new position
// of an object after the specified time has elapsed.
// If the Ship is going to move
// for a full time unit (one hour), then it will
// get go the "full step" distance,
// so update_position would be called with
// time = 1.0. If we can move less than that,
// e.g. due to not enough fuel, update position
// will be called with the corresponding
// time less than 1.0.
//
// For clarity in specifying the computation,
// this code assumes the specified private variable names, 
// but you may change the variable names or
// state names if you wish (e.g. movement_state).
void
Ship::calculate_movement()
{
    // Compute values for how much we need to move,
    // and how much we can, and how long we can,
    // given the fuel state, then decide what to do.

    // full step time
    double time = 1.0;

    // get the distance to destination
    double destination_distance = cartesian_distance(get_location(),
                                                     destination);

    // get full step distance we can move on this time step
    double full_distance = track_base.get_speed() * time;

    // get fuel required for full step distance
    double full_fuel_required = full_distance * fuel_consumption;

    // how far and how long can we sail
    // in this time period based on the fuel state?
    double distance_possible, time_possible;

    if (full_fuel_required <= fuel)
    {
        distance_possible = full_distance;
        time_possible = time;
    }
    else
    {
        // nm = tons / tons/nm
        distance_possible = fuel / fuel_consumption;
        time_possible = (distance_possible / full_distance) * time;
    }

    // are we moving to a destination,
    // and is the destination within the distance possible?
    if (ship_state == MOVING_TO_POSITION &&
        destination_distance <= distance_possible)
    {
        // yes, make our new position the destination
        track_base.set_position(destination);

        // we travel the destination distance, using that much fuel
        double fuel_required = destination_distance * fuel_consumption;
        fuel -= fuel_required;
        track_base.set_speed(0.);
        ship_state = STOPPED;
    }
    else
    {
        // go as far as we can, stay in the same movement state
        // simply move for the amount of time possible
        track_base.update_position(time_possible);

        // have we used up our fuel?
        if (full_fuel_required >= fuel)
        {
            fuel = 0.0;
            ship_state = DEAD_IN_THE_WATER;
        }
        else
        {
            fuel -= full_fuel_required;
        }
    }
}

void
Ship::check_speed_and_move(double speed)
{
    if (!can_move())
    {
        throw Error("Ship cannot move!");
    }
    if (speed > maximum_speed)
    {
        throw Error("Ship cannot go that fast!");
    }
}
