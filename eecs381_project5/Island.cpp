#include "Island.h"
#include "Model.h"

using std::cout;
using std::string;
using std::endl;

Island::Island(const string& name_,
               Point position_,
               double fuel_,
               double production_rate_) :
    Sim_object(name_),
    position(position_),
    fuel(fuel_),
    production_rate(production_rate_)
{
    cout << "Island " << get_name() << " constructed" << endl;
}

Island::~Island()
{
    cout << "Island " << get_name() << " destructed" << endl;
}

double
Island::provide_fuel(double request)
{
    // there is not enough fuel to fulfill request
    if (fuel < request)
    {
        request = fuel;
        fuel = 0.;
    }
    // there is enough fuel to fulfill request
    else
    {
        fuel -= request;
    }

    cout << "Island " << get_name() << " supplied "
         << request << " tons of fuel" << endl;

    return request;
}

void
Island::accept_fuel(double amount)
{
    fuel += amount;
    cout << "Island " << get_name()  << " now has "
         << fuel  << " tons" << endl;
}

void
Island::update()
{
    if (production_rate > 0)
    {
        fuel += production_rate * 1.0;
        cout << "Island " << get_name()  << " now has "
             << fuel  << " tons" << endl;
    }
}

void
Island::describe() const
{
    cout << "\nIsland " << get_name()
         << " at position " << position << endl
         << "Fuel available: " << fuel << " tons" << endl;
}

// ask model to notify views of current state
void
Island::broadcast_current_state()
{
    Model::get_Instance().notify_location(get_name(), position);
}
