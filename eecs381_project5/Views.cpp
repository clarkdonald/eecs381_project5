#include "Views.h"
#include "Utility.h"
#include <iostream>
#include <cmath>
#include <list>
#include <vector>
#include <algorithm>
#include <iomanip>

using namespace std;

Map_View::Map_View() :
    size(25),
    scale(2.0),
    origin(Point(-10.,-10.))
{}

Map_View::~Map_View()
{
    name_location_map.clear();
}

void
Map_View::update_location(const string& name, Point location)
{
    name_location_map[name] = location;
}

void
Map_View::update_remove(const string& name)
{
    map<std::string, Point>::iterator it;
    
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
    vector< vector<std::string> > grid;
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
Map_View::clear()
{
    name_location_map.clear();
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
    if (scale_ <= 0.0)
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
    scale  = 2.0;
    origin = Point(-10.,-10.);
}

// Calculate the cell subscripts corresponding
// to the supplied location parameter,
// using the current size, scale, and origin of the display.
// This function assumes that origin is a
// member variable of type Point,
// scale is a double value, and size is an
// integer for the number of rows/columns
// currently being used for the grid.
// Return true if the location
// is within the grid, false if not
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

