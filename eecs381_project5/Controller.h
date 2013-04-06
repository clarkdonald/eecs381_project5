#ifndef CONTROLLER_H
#define CONTROLLER_H

/*******************************************************
    Controller
    This class is responsible for controlling the Model
    and View according to interactions with the user.
*******************************************************/

#include <map>
#include <string>
#include <memory>
#include <vector>

class View;
class Ship;

class Controller
{
  public:       
      // constructor
      Controller();

      // destructor
      ~Controller();

      // create View object,
      // run the program by acccepting user commands,
      // then destroy View object
      void run();

  private:
      // command functions
      void view_default(std::shared_ptr<View>);
      void view_size(std::shared_ptr<View>);
      void view_zoom(std::shared_ptr<View>);
      void view_pan(std::shared_ptr<View>);
      void view_show();
      void model_status();
      void model_go();
      void model_create();
      void ship_course(std::shared_ptr<Ship>);
      void ship_position(std::shared_ptr<Ship>);
      void ship_destination(std::shared_ptr<Ship>);
      void ship_load_at(std::shared_ptr<Ship>);
      void ship_unload_at(std::shared_ptr<Ship>);
      void ship_dock_at(std::shared_ptr<Ship>);
      void ship_attack(std::shared_ptr<Ship>);
      void ship_refuel(std::shared_ptr<Ship>);
      void ship_stop(std::shared_ptr<Ship>);
      void ship_stop_attack(std::shared_ptr<Ship>);
      void open_map_view(std::shared_ptr<View>);
      void close_map_view(std::shared_ptr<View>);
      void open_sailing_view(std::shared_ptr<View>);
      void close_sailing_view(std::shared_ptr<View>);
      void open_bridge_view();
      void close_bridge_view();

      // error check functions and helpers
      double receive_and_check_speed();
      std::string receive_and_check_island();
      std::string receive_and_check_ship();
      void open_view(std::shared_ptr<View>, const std::string &);
      void close_view(std::shared_ptr<View>, const std::string &);
    
      // maps for ship, model, view commands
      std::map<std::string, void (Controller::*)(std::shared_ptr<Ship>)>
          ship_command_map;
      std::map<std::string, void (Controller::*)(std::shared_ptr<View>)>
          map_command_map;
      std::map<std::string, void (Controller::*)(std::shared_ptr<View>)>
          sailing_command_map;
      std::map<std::string, void (Controller::*)()> no_arg_command_map;
    
      // map of views
      std::vector<std::shared_ptr<View>> view_container;
      std::map<std::string, std::shared_ptr<View>> bridge_map;
};

#endif
