#ifndef VIEWS_H
#define VIEWS_H

#include "Geometry.h"
#include <string>
#include <map>

class View
{
  public:
      View();
      virtual ~View() = 0;
    
      // Save the supplied name and location
      // for future use in a draw() call
      // If the name is already present,
      // the new location replaces the previous one.
      virtual void update_location(const std::string& name, Point location) = 0;
    
      // Save supplied name and fuel for future use in draw() call
      virtual void update_fuel(const std::string& name, double fuel) = 0;
    
      // Save supplied name and speed for future use in draw() call
      virtual void update_speed(const std::string& name, double speed) = 0;
    
      // Save supplied name and course for future use in draw() call
      virtual void update_course(const std::string& name, double course) = 0;
    
      // Remove the name and its location;
      // no error if the name is not present.
      virtual void update_remove(const std::string& name) = 0;
    
      // prints out the current map
      virtual void draw() = 0;
    
      // Discard the saved information -
      // drawing will show only a empty pattern
      virtual void clear() = 0;
};

class Map_View : public View
{
  public:
      // default constructor sets the default size, scale, and origin
      Map_View();
      ~Map_View()
          {name_location_map.clear();}
    
      // Save the supplied name and location
      // for future use in a draw() call
      // If the name is already present,
      // the new location replaces the previous one.
      void update_location(const std::string& name, Point location) override
          {name_location_map[name] = location;}
    
      // Remove the name and its location;
      // no error if the name is not present.
      virtual void update_remove(const std::string& name) override;
    
      // prints out the current map
      virtual void draw() override;
    
      // Discard the saved information -
      // drawing will show only a empty pattern
      virtual void clear() override
          {name_location_map.clear();}
    
      // modify the display parameters
      // if the size is out of bounds
      // will throw Error("New map size is too big!")
      // or Error("New map size is too small!")
      void set_size(int size_);
    
      // If scale is not postive,
      // will throw Error("New map scale must be positive!");
      void set_scale(double scale_);
    
      // any values are legal for the origin
      void set_origin(Point origin_);
    
      // set the parameters to the default values
      void set_defaults();
    
  private:
      int size;       // current size of the display
      double scale;   // distance per cell of the display
      Point origin;   // coordinates of the lower-left-hand corner
      std::map<std::string, Point> name_location_map;
    
      // Calculate the cell subscripts
      // corresponding to the location parameter, using the
      // current size, scale, and origin of the display.
      // Return true if the location is within the map, false if not
      bool get_subscripts(int &ix, int &iy, Point location);
};

class Sailing_View : public View
{
  public:
      Sailing_View();
      ~Sailing_View();
    
      // Save supplied name and fuel for future use in draw() call
      virtual void update_fuel(const std::string& name, double fuel) override
          {fuel_map[name] = fuel;}
    
      // Save supplied name and speed for future use in draw() call
      virtual void update_speed(const std::string& name, double speed) override
          {speed_map[name] = speed;}
    
      // Save supplied name and course for future use in draw() call
      virtual void update_course(const std::string& name, double course) override
          {course_map[name] = course;}
    
      // Remove the name and its location;
      // no error if the name is not present.
      virtual void update_remove(const std::string& name) override;
    
      // prints out the current map
      virtual void draw() override;
    
      // Discard the saved information -
      // drawing will show only a empty pattern
      virtual void clear() override;
    
  private:
      std::map<std::string, double> fuel_map;
      std::map<std::string, double> speed_map;
      std::map<std::string, double> course_map;
};

class Bridge_View : public View
{
  public:
      Bridge_View();
      ~Bridge_View();
    
      // Remove the name and its location;
      // no error if the name is not present.
      virtual void update_remove(const std::string& name) override;
    
      // prints out the current map
      virtual void draw() override;

      // Discard the saved information -
      // drawing will show only a empty pattern
      virtual void clear() override;
    
  private:
};

#endif




























