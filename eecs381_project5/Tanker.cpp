#include "Tanker.h"
#include "Island.h"
#include "Utility.h"

using std::string;
using std::cout;
using std::endl;

Tanker::Tanker(const std::string& name_, Point position_) :
    Ship(name_, position_, 100., 10., 2., 0),
    cargo(0.0),
    cargo_capacity(1000.),
    load_destination(nullptr),
    unload_destination(nullptr),
    tanker_state(NO_CARGO_DESTINATION)
{
    cout << "Tanker " << get_name() << " constructed" << endl;
}

Tanker::~Tanker()
{
    cout << "Tanker " << get_name() << " destructed" << endl;
}

void
Tanker::set_destination_position_and_speed(Point destination, double speed)
{
    if (tanker_state != NO_CARGO_DESTINATION)
    {
        throw Error("Tanker has cargo destinations!");
    }
    Ship::set_destination_position_and_speed(destination, speed);
}

void 
Tanker::set_course_and_speed(double course, double speed)
{
    if (tanker_state != NO_CARGO_DESTINATION)
    {
        throw Error("Tanker has cargo destinations!");
    }
    Ship::set_course_and_speed(course, speed);
}

void
Tanker::set_load_destination(Island *island_ptr)
{
    if (tanker_state != NO_CARGO_DESTINATION)
    {
        throw Error("Tanker has cargo destinations!");
    }

    load_destination = island_ptr;

    if (load_destination == unload_destination)
    {
        throw Error("Load and unload cargo destinations are the same!");
    }

    cout << get_name() << " will load at " 
         << load_destination->get_name() << endl;

    tanker_cycle();
}

void
Tanker::set_unload_destination(Island *island_ptr)
{
    if (tanker_state != NO_CARGO_DESTINATION)
    {
        throw Error("Tanker has cargo destinations!");
    }

    unload_destination = island_ptr;

    if (load_destination == unload_destination)
    {
        throw Error("Load and unload cargo destinations are the same!");
    }

    cout << get_name() << " will unload at "
         << unload_destination->get_name() << endl;

    tanker_cycle();
}

void
Tanker::stop()
{
    Ship::stop();
    reset_state();
    cout << get_name() << " now has no cargo destinations" << endl;
}

void
Tanker::update()
{
    Ship::update();

    if (!can_move())
    {
        if (tanker_state != NO_CARGO_DESTINATION)
        {
            reset_state();
            cout << get_name() << " now has no cargo destinations" << endl;
        }
        return;
    }

    if (tanker_state == NO_CARGO_DESTINATION)
    {
        return;
    }
    if (tanker_state == MOVING_TO_LOADING)
    {
        if (!is_moving() && can_dock(load_destination))
        {
            dock(load_destination);
            tanker_state = LOADING;
            return;
        }
    }
    if (tanker_state == MOVING_TO_UNLOADING)
    {
        if (!is_moving() && can_dock(unload_destination))
        {
            dock(unload_destination);
            tanker_state = UNLOADING;
            return;
        }
    }
    if (tanker_state == LOADING)
    {
        Ship::refuel();

        double fuel_needed_to_fill_cargo = cargo_capacity - cargo;
        if (fuel_needed_to_fill_cargo < 0.005)
        {
            cargo = cargo_capacity;
            Ship::set_destination_position_and_speed(unload_destination->get_location(),
                                                     get_maximum_speed());
            tanker_state = MOVING_TO_UNLOADING;
            return;
        }
        else
        {
            cargo += load_destination->provide_fuel(fuel_needed_to_fill_cargo);
            cout << get_name() << " now has " << cargo << " of cargo" << endl;
            return;
        }
    }
    if (tanker_state == UNLOADING)
    {
        if (cargo == 0.0)
        {
            Ship::set_destination_position_and_speed(load_destination->get_location(),
                                                     get_maximum_speed());
            tanker_state = MOVING_TO_LOADING;
            return;
        }
        else
        {
            unload_destination->accept_fuel(cargo);
            cargo = 0.0;
            return;
        }
    }
}

void
Tanker::describe() const
{
    cout << "\nTanker ";
    Ship::describe();
    cout << "Cargo: " << cargo << " tons";

    switch (tanker_state)
    {
        case NO_CARGO_DESTINATION:
            cout << ", no cargo destinations";
            break;
        case LOADING:
            cout << ", loading";
            break;
        case UNLOADING:
            cout << ", unloading";
            break;
        case MOVING_TO_LOADING:
            cout << ", moving to loading destination";
            break;
        case MOVING_TO_UNLOADING:
            cout << ", moving to unloading destination";
            break;
    }
    cout << endl;
}

void
Tanker::tanker_cycle()
{
    if (!load_destination || !unload_destination)
    {
        return;
    }
    if (is_docked())
    {
        if (get_docked_Island() == load_destination)
        {
            tanker_state = LOADING;
            return;
        }
        else if (get_docked_Island() == unload_destination)
        {
            tanker_state = UNLOADING;
            return;
        }
    }
    if (!is_moving())
    {
        if (cargo == 0.0 && can_dock(load_destination))
        {
            dock(load_destination);
            tanker_state = LOADING;
            return;
        }
        else if (cargo > 0.0 && can_dock(unload_destination))
        {
            dock(unload_destination);
            tanker_state = UNLOADING;
            return;
        }
    }
    if (cargo == 0.0)
    {
        set_destination_position_and_speed(load_destination->get_location(),
                                           get_maximum_speed());
        tanker_state = MOVING_TO_LOADING;
        return;
    }
    if (cargo > 0.0)
    {
        set_destination_position_and_speed(unload_destination->get_location(),
                                           get_maximum_speed());
        tanker_state = MOVING_TO_UNLOADING; 
        return;
    }
}

void
Tanker::reset_state()
{
    tanker_state       = NO_CARGO_DESTINATION;
    load_destination   = nullptr;
    unload_destination = nullptr;
}
