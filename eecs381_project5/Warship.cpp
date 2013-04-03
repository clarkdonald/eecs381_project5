#include "Warship.h"
#include "Utility.h"

using namespace std;

// default value for warship state is not attacking
Warship::Warship(const string& name_,
                 Point position_,
                 double fuel_capacity_, 
                 double maximum_speed_,
                 double fuel_consumption_factor_,
                 int resistance_,
                 int firepower_,
                 double maximum_range_) :
    Ship(name_,
         position_,
         fuel_capacity_,
         maximum_speed_,
         fuel_consumption_factor_,
         resistance_),
    firepower(firepower_),
    maximum_range(maximum_range_),
    warship_state(NOT_ATTACKING)
{}

Warship::~Warship()
{}

void
Warship::update()
{
    Ship::update();

    if (is_attacking())
    {
        shared_ptr<Ship> target = target_ptr.lock();
        
        if (!is_afloat() || !target || !target->is_afloat())
        {
            stop_attack();
        }
        else
        {
            cout << get_name() << " is attacking " << endl;
        }
    }
}

void
Warship::attack(shared_ptr<Ship> target_ptr_)
{
    if (!is_afloat())
    {
        throw Error("Cannot attack!");
    }

    if (target_ptr_->get_name() == get_name())
    {
        throw Error("Warship may not attack itself!");
    }

    target_ptr    = target_ptr_;
    warship_state = ATTACKING;

    cout << get_name() << " will attack "
         << target_ptr_->get_name() << endl;
}

void
Warship::stop_attack()
{
    if (!is_attacking())
    {
        throw Error("Was not attacking!");
    }

    warship_state = NOT_ATTACKING;
    target_ptr.reset();

    cout << get_name() << " stopping attack" << endl;
}

void
Warship::describe() const
{
    Ship::describe();
    
    shared_ptr<Ship> target = target_ptr.lock();

    if (is_attacking())
    {
        if (!target || !target->is_afloat())
        {
            cout << "Attacking absent ship" << endl;
        }
        else
        {
            cout << "Attacking " <<  target->get_name() << endl;
        }
    }
}

bool
Warship::is_attacking() const
{
    if (warship_state == ATTACKING)
    {
        return true;
    }

    return false;
}

void
Warship::fire_at_target()
{
    shared_ptr<Ship> target = target_ptr.lock();
    
    if (target)
    {
        cout << get_name() << " fires" << endl;

        target->receive_hit(firepower, shared_from_this());
    }
}

bool
Warship::target_in_range() const
{
    shared_ptr<Ship> target = target_ptr.lock();
    
    if (target &&
        cartesian_distance(get_location(), target->get_location()) >
        maximum_range)
    {
        return false;
    }

    return true;
}
