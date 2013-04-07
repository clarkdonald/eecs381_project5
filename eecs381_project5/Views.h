#ifndef VIEWS_H
#define VIEWS_H

#include <string>
#include <map>
#include <memory>

struct Point;
class Ship;

class View
{
  public:
      View();
      virtual ~View() = 0;
      virtual void update_location(const std::string&, Point) {}
      virtual void update_fuel(const std::string&, double) {}
      virtual void update_speed(const std::string&, double) {}
      virtual void update_course(const std::string&, double) {}
      virtual void update_remove(const std::string&) = 0;
      virtual void draw() = 0;
      virtual void clear() = 0;
      virtual void set_size(int) {}
      virtual void set_scale(double) {}
      virtual void set_origin(Point) {}
      virtual void set_defaults() {}
};

class Map_View : public View
{
  public:
      // default constructor sets the default size, scale, and origin
      Map_View();
      ~Map_View();
    
      // Save the supplied name and location
      // for future use in a draw() call
      // If the name is already present,
      // the new location replaces the previous one.
      void update_location(const std::string& name, Point location) override
          {name_location_map[name] = location;}
    
      // Remove the name and its location;
      // no error if the name is not present.
      void update_remove(const std::string&) override;
    
      // prints out the current map
      void draw() override;
    
      // Discard the saved information -
      // drawing will show only a empty pattern
      void clear() override
          {name_location_map.clear();}
    
      // modify the display parameters
      // if the size is out of bounds
      // will throw Error("New map size is too big!")
      // or Error("New map size is too small!")
      void set_size(int);
    
      // If scale is not postive,
      // will throw Error("New map scale must be positive!");
      void set_scale(double);
    
      // any values are legal for the origin
      void set_origin(Point);
    
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
      void update_fuel(const std::string&, double) override;
    
      // Save supplied name and speed for future use in draw() call
      void update_speed(const std::string&, double) override;
    
      // Save supplied name and course for future use in draw() call
      void update_course(const std::string&, double) override;
    
      // Remove the name and its location;
      // no error if the name is not present.
      void update_remove(const std::string&) override;
    
      // prints out the current map
      void draw() override;
    
      // Discard the saved information -
      // drawing will show only a empty pattern
      void clear() override
          {ship_data_map.clear();}
    
  private:
      struct Ship_Data
      {
          double fuel;
          double speed;
          double course;
      };
      std::map<std::string, Ship_Data> ship_data_map;
};

class Bridge_View : public View
{
  public:
      Bridge_View(const std::string&, Point, double, bool);
      ~Bridge_View();
    
      // Save the supplied name and location
      // for future use in a draw() call
      // If the name is already present,
      // the new location replaces the previous one.
      void update_location(const std::string&, Point) override;
    
      // Save supplied name and course for future use in draw() call
      void update_course(const std::string&, double) override;
    
      // Remove the name and its location;
      // no error if the name is not present.
      void update_remove(const std::string& name) override;
    
      // prints out the current map
      void draw() override;

      // Discard the saved information -
      // drawing will show only a empty pattern
      void clear() override
          {name_location_map.clear();}
    
  private:
      int x_size;     // grid length of x-coordinate
      int y_size;     // grid length of y-coordinate
      double scale;   // distance per cell of the display
      double origin;   // coordinates of the lower-left-hand corner
      struct Ownship
      {
          std::string name;
          Point location;
          double heading;
          bool sunk;
      } ownship;
      std::map<std::string, Point> name_location_map;
    
      // Calculate the x-axis displacement
      // corresponding to the location parameter, using the
      // current size, scale, and origin of the display.
      // Return true if the location is within the map, false if not
      bool get_x_coordinate(int&, double);
    
      // calculate angle needed to see object from ship's bow
      double calc_angle(Point other);
};

#endif




























