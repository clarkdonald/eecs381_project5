#include "Ship_factory.h"
#include "Cruiser.h"
#include "Tanker.h"
#include "Utility.h"

using namespace std;

shared_ptr<Ship>
create_ship(const string& name,
            const string& type,
            Point initial_position)
{
    if (type == "Cruiser")
    {
        shared_ptr<Ship> return_ship(new Cruiser(name, initial_position));
        return return_ship;
    }
    else if (type == "Tanker")
    {
        shared_ptr<Ship> return_ship(new Tanker(name, initial_position));
        return return_ship;
    }
    else
    {
        throw Error("Trying to create ship of unknown type!");
    }
}
