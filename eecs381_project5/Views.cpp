#include "Geometry.h"
#include "Navigation.h"
#include "Views.h"
#include "Utility.h"
#include <iostream>
#include <cmath>
#include <list>
#include <vector>
#include <algorithm>
#include <iomanip>

using namespace std;

/// VIEW /////////////////////////////////////////////////////////
View::View() {}
View::~View() {}
//////////////////////////////////////////////////////////////////


/// MAP VIEW /////////////////////////////////////////////////////
Map_View::Map_View() :
    size(25),
    scale(2.),
    origin(Point(-10.,-10.))
{}

Map_View::~Map_View()
{
    clear();
}

void
Map_View::update_remove(const string& name)
{
    map<string, Point>::iterator it;
    
    if ((it = name_location_map.find(name)) !=
        name_location_map.end())
    {
        name_location_map.erase(it);
    }
}

// prints out the current map
void
Map_View::draw()
{
    // 3D grid is local to draw
    vector< vector<string> > grid;
    grid.resize(size, vector<string>(size, ". "));
    
    cout << "Display size: " << size
         << ", scale: " <<  scale
         << ", origin: " <<  origin << endl;
    
    // place objects into grid
    list<string> outside_members;
    for_each(name_location_map.begin(),
             name_location_map.end(),
             [&outside_members, this, &grid](pair<string, Point> obj)
             {
                 int x, y;
                 
                 // place object into grid if in range
                 if (get_subscripts(x, y, obj.second))
                 {
                     if (grid[y][x] == ". ")
                     {
                         grid[y][x] = obj.first.substr(0,2);
                     }
                     else
                     {
                         grid[y][x] = "* ";
                     }
                 }
                 // print out of range message
                 else
                 {
                     outside_members.push_back(obj.first);
                 }
             });
    
    // print outside members, if they exist
    int num_members = static_cast<int>(outside_members.size());
    if (num_members > 0)
    {
        for_each(outside_members.begin(),
                 outside_members.end(),
                 [&num_members](string name)
                 {
                     cout << name;
                     if (num_members-- > 1)
                     {
                         cout << ", ";
                     }
                 });
        
        cout << " outside the map" << endl;
    }
    
    // print grid content to stdout
    int axis = origin.y + scale*(size-1);
    for (int i = 0; i < size; ++i)
    {
        // check if we need to
        // print out the y-coordinate axis number
        if (!((size-1-i) % 3))
        {
            cout << setw(4) << axis << ' ';
        }
        else
        {
            cout << "     ";
        }
        axis -= scale;
        
        for (int j = 0; j < size; ++j)
        {
            // print grid content
            cout << grid[size-1-i][j];
        }
        cout << endl;
    }
    
    // print x-coordinate axis values
    axis = origin.x;
    for (int i = 0; i < size; i += 3)
    {
        cout << setw(6) << axis;
        axis += 3*scale;
    }
    
    cout << endl;
}

void
Map_View::set_size(int size_)
{
    if (size_ > 30)
    {
        throw Error("New map size is too big!");
    }
    else if (size_ <= 6)
    {
        throw Error("New map size is too small!");
    }
    
    size = size_;
}

void
Map_View::set_scale(double scale_)
{
    if (scale_ <= 0.)
    {
        throw Error("New map scale must be positive!");
    }
    
    scale = scale_;
}

void
Map_View::set_origin(Point origin_)
{
    origin = origin_;
}

void
Map_View::set_defaults()
{
    size   = 25;
    scale  = 2.;
    origin = Point(-10.,-10.);
}

bool
Map_View::get_subscripts(int &ix, int &iy, Point location)
{
    // adjust with origin and scale
    Cartesian_vector subscripts = (location - origin) / scale;
    
    // truncate coordinates to integer after taking the floor
    // floor function will produce integer
    // smaller than even for negative values,
    // so - 0.05 => -1., which will be outside the array.
    ix = int(floor(subscripts.delta_x));
    iy = int(floor(subscripts.delta_y));
    
    // if out of range, return false
    if ((ix < 0) || (ix >= size) || (iy < 0) || (iy >= size))
    {
        return false;
    }
    else
    {
        return true;
    }
}
//////////////////////////////////////////////////////////////////


/// SAILING VIEW /////////////////////////////////////////////////
Sailing_View::Sailing_View()
{}

Sailing_View::~Sailing_View()
{
    clear();
}

void
Sailing_View::update_fuel(const std::string& name, double fuel)
{
    map<string, Ship_Data>::iterator it;
    
    if ((it = ship_data_map.find(name)) != ship_data_map.end())
    {
        it->second.fuel = fuel;
    }
    else
    {
        ship_data_map[name] = {fuel,0.,0.};
    }
}

void
Sailing_View::update_speed(const std::string& name, double speed)
{
    map<string, Ship_Data>::iterator it;
    
    if ((it = ship_data_map.find(name)) != ship_data_map.end())
    {
        it->second.speed = speed;
    }
    else
    {
        ship_data_map[name] = {0.,speed,0.};
    }
}

void
Sailing_View::update_course(const std::string& name, double course)
{
    map<string, Ship_Data>::iterator it;
    
    if ((it = ship_data_map.find(name)) != ship_data_map.end())
    {
        it->second.course = course;
    }
    else
    {
        ship_data_map[name] = {0.,0.,course};
    }
}

void
Sailing_View::update_remove(const string& name)
{
    map<string, Ship_Data>::iterator it;
    
    if ((it = ship_data_map.find(name)) != ship_data_map.end())
    {
        ship_data_map.erase(it);
    }
}

void
Sailing_View::draw()
{
    cout << "----- Sailing Data -----" << endl;
    cout << setw(10) << "Ship" << setw(10) << "Fuel"
         << setw(10) << "Course" << setw(10) << "Speed" << endl;
    
    for_each(ship_data_map.begin(),
             ship_data_map.end(),
             [this](pair<string, Ship_Data> obj)
             {
                 cout << setw(10) << obj.first
                      << setw(10) << obj.second.fuel
                      << setw(10) << obj.second.course
                      << setw(10) << obj.second.speed << endl;
             });
}
//////////////////////////////////////////////////////////////////


/// BRIDGE VIEW //////////////////////////////////////////////////
Bridge_View::Bridge_View(const string &name_,
                         Point location_,
                         double heading_,
                         bool sunk_) :
    x_size(19),
    y_size(3),
    scale(10.),
    origin(-90.),
    ownship({name_, location_, heading_, sunk_})
{}

Bridge_View::~Bridge_View()
{
    clear();
}

void
Bridge_View::update_location(const std::string& name_, Point location_)
{
    if (name_ == ownship.name)
    {
        ownship.location = location_;
    }
    else
    {
        map<string, Point>::iterator it;
        
        if ((it = name_location_map.find(name_)) !=
            name_location_map.end())
        {
            it->second = location_;
        }
        else
        {
            name_location_map[name_] = location_;
        }
    }
}

void
Bridge_View::update_course(const std::string& name_, double heading_)
{
    if (name_ == ownship.name)
    {
        ownship.heading = heading_;
    }
}

void
Bridge_View::update_remove(const std::string& name_)
{
    // check if removed is ownship
    if (name_ == ownship.name)
    {
        ownship.sunk = true;
        clear();
    }
    else
    {
        map<string, Point>::iterator it;
        
        if ((it = name_location_map.find(name_)) !=
            name_location_map.end())
        {
            name_location_map.erase(it);
        }
    }
}

void
Bridge_View::draw()
{
    if (ownship.sunk)
    {
        cout << "Bridge view from " << ownship.name
             << " sunk at " << ownship.location << endl;
        
        for (int i = 0; i < y_size; ++i)
        {
            cout << "     w-w-w-w-w-w-w-w-w-w-w-w-w-w-w-w-w-w-w-"
                 << endl;
        }
    }
    else
    {
        // store ship/island locations for printing
        vector<string> grid(x_size, ". ");
        
        cout << "Bridge view from " << ownship.name << " position "
             << ownship.location << " heading "
             << ownship.heading << endl;
        
        // place objects into grid after bearing calculation
        for_each(name_location_map.begin(),
                 name_location_map.end(),
                 [this, &grid](pair<string, Point> obj)
                 {
                     double distance = cartesian_distance(ownship.location, obj.second);
                     
                     if (distance <= 20. && distance >= 0.005)
                     {
                         int x;
                         
                         // place object into grid if in range
                         if (get_x_coordinate(x, calc_angle(obj.second)))
                         {
                             if (grid[x] == ". ")
                             {
                                 grid[x] = obj.first.substr(0,2);
                             }
                             else
                             {
                                 grid[x] = "**";
                             }
                         }
                     }
                 });

        // print grid content to stdout
        for (int i = 0; i < y_size - 1; ++i)
        {
            cout << "     . . . . . . . . . . . . . . . . . . . "
                 << endl;
        }
        cout << "     ";
        for (int i = 0; i < x_size; ++i)
        {
            cout << grid[i];
        }
        cout << endl;
    }
    
    // print x-coordinate axis values
    int axis = static_cast<int>(origin);
    for (int i = 0; i < x_size; i += 3)
    {
        cout << setw(6) << axis;
        axis += 3*static_cast<int>(scale);
    }
    cout << endl;
}

bool
Bridge_View::get_x_coordinate(int &ix, double angle)
{
    // adjust with origin and scale
    double delta_x = (angle - origin) / scale;
    
    // truncate delta x to integer after taking the floor
    // floor function will produce integer
    // smaller than even for negative values,
    // so - 0.05 => -1., which will be outside the array.
    ix = int(floor(delta_x));
    
    // if out of range, return false
    if ((ix < 0) || (ix >= x_size))
    {
        return false;
    }
    else
    {
        return true;
    }
}

double
Bridge_View::calc_angle(Point other)
{
    Compass_position compass_position(ownship.location, other);
    
    double angle = compass_position.bearing - ownship.heading;
    
    if (angle < -180.)
    {
        angle += 360.;
    }
    else if (angle > 180.)
    {
        angle -= 360.;
    }
    
    return angle;
}
//////////////////////////////////////////////////////////////////















