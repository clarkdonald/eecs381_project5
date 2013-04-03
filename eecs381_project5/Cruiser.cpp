#include "Cruiser.h"

using namespace std;

Cruiser::Cruiser(const std::string& name_, Point position_) :
    Warship(name_, position_, 1000., 20., 10., 6, 3, 15)          
{
    cout << "Cruiser " << get_name() << " constructed" << endl;
}

// output destructor message
Cruiser::~Cruiser()
{
    cout << "Cruiser " <<  get_name() << " destructed" << endl;
}

void
Cruiser::update()
{
    Warship::update();

    if (is_attacking())
    {
        if (target_in_range())
        {
            fire_at_target();
        }
        else
        {
            cout << get_name()
                 << " target is out of range" << endl;
            stop_attack();
        }
    }
}

void
Cruiser::describe() const
{
    cout << "\nCruiser ";
    Warship::describe();
}

void
Cruiser::receive_hit(int hit_force, shared_ptr<Ship> attacker_ptr)
{
    Ship::receive_hit(hit_force, attacker_ptr);
    if (!is_attacking())
    {
        Warship::attack(attacker_ptr);
    }
}
