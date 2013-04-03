#ifndef TRACK_BASE_H
#define TRACK_BASE_H

/******************************************************
    The Track_base class defines a base class
    of track objects, which are objects that move
    according to course and speed. They have a Point,
    a Course_speed, and an altitude 
    (set to zero for surface tracks). When updated,
    they change their Point 
    as a function of their Course_speed.

    Various values can be calculated for this track's
    position or motion as viewed from
    some other track.
******************************************************/

#include "Geometry.h"
#include "Navigation.h"

class Track_base
{
  public:
      // Constructors/Destructors
      Track_base();
      Track_base(Point in_position);
      Track_base(Point in_position,
                 Course_speed in_course_speed,
                 double in_altitude = 0.);
      virtual ~Track_base();

      // Readers
      Point get_position() const 
              {return position;}
      Course_speed get_course_speed() const 
              {return course_speed;}
      double get_course() const 
              {return course_speed.course;}
      double get_speed() const 
              {return course_speed.speed;}
      double get_altitude() const
              {return altitude;}

      // Writers
      void set_position(Point in_position)
              {position = in_position;}
      void set_course_speed(const Course_speed& in_course_speed)
              {course_speed = in_course_speed;}
      void set_course (double in_course)
              {course_speed.course = in_course;}
      void set_speed (double in_speed)
              {course_speed.speed = in_speed;}
      void set_altitude (double in_altitude)
              {altitude = in_altitude;}

      /*
          Calculate track motion analysis results from this track and a supplied 
          other track or position - the other track is normally "ownship", so
          these look "backwards" at first glance
      */

      // range and bearing of this track from a specified position
      Compass_position get_range_and_bearing_from(const Point & p) const;

      // range and bearing of this track from a specified other track  
      Compass_position get_range_and_bearing_from(const Track_base * track_ptr) const;

      // range and bearing of closet point of approach of this track from a specified track
      Compass_position get_CPA_range_and_bearing_from(const Track_base * track_ptr) const;

      // calculate whether this track is inbound towards a specified track
      bool is_inbound_toward(const Track_base * track_ptr) const;

      // Update the position of this object using the supplied time increment
      // which is multiplied by the speed to get the distance to be moved
      virtual void update_position(double time_increment);

  private:
      Point position;
      Course_speed course_speed;
      double altitude;
};

#endif
