#ifndef NAVIGATION_H
#define NAVIGATION_H

/*****************************************************************
This set of simple classes is used to describe
and compute positions, courses, and 
changes to positions and courses in normal
Navigation domain expressions.
The classes are defined with "struct" to
make all members public by default.

These classes and functions implement "plane sailing"
in which movement is assumed 
to be on a plane, and so the rules of plane geometry
and trigonometry apply. In practice,
this is quite accurate at reasonably small distances.
Large-scale navigation must take
the almost-spherical shape of the Earth into account,
where position is represented in 
(latitude,longitude) coordinates in degrees measured
North and South of the equator and 
East and West of the Prime Meridian. Computations on
this scale require spherical 
trigonometry to be acceptably accurate, and much more
complex computations are involved. 
For example, converting to/from (latitude,longitude)
and Points entails 
taking the latitude into account to determining the
difference in longitude and the 
Point origin that is being assumed. In general, this
entails assumptions about 
the projection from  spherical earth coordinates to a
plane, and thus does 
not have a general solution. The lat/lon classes such
as Geo_position and the related functions 
for great_circle course calculations have been removed
from this version of the Navigation module, 
leaving only the Point class and functions for "plane sailing".

A Point is represented in (x, y) coordinates
in an nautical mile (nm) grid 
whose origin is (0, 0). Points can be compared
with == and !=, 
but since the values are doubles, equality
should only be expected to result 
from assignment, not computation. 

Angles in Navigation are expressed in Compass degrees,
where North is 0 degrees and East is 90 degrees,
South is 180 degrees, and West is 270 degrees.
North is not 360 degrees, but is expressed as 0 degrees.
If North corresponds to "up" and East to "right"
(as on standard map directions), 
then Navigation or Compass degrees to do not correspond
to the normal geometry/trig representation 
in terms of quadrants where "up" is 90 degrees and "right"
is zero degrees. This module includes 
functions for converting between the two
systems of angle measurement.

A Compass_position is represented as (bearing, range).
This is normally relative to ownship.
It can be constructed from two Points or a Polar_vector.
The angle in Compass_positions is in terms of 
"Compass" or "Navigation" degrees, where North is 0 degrees and
East is 90 degrees. 

A Course_speed is (course, speed) -
used to represent an object's course as normally
expressed as a direction in compass degrees and a
speed in nm/hr. It is an "input" class only - 
no constructors are provided from other classes.
  
A Compass_vector is (course, distance) -
shows a displacement along a compass course for
a certain distance. 

To determine where a track is given its relation 
to ownship, add ownship's Point and 
the track's Compass_position get the track's Point: 
    ownship_position + track_compass_position => track_position
        
To determine the displacement produced by following 
a course for some time,
multiply the Course_speed by the time (a double) 
to get a Compass_vector.
    track_course_speed * time => compass_vector
    time * track_course_speed => compass_vector

To determine where a track will be, add its current Point and 
the Compass_vector for its course
and the time to get its new Point:
    current_track_position +
    track_compass_vector => new_track_position
******************************************************************/

#include "Geometry.h"
#include <iosfwd>

// forward declarations
struct Course_speed;
struct Compass_position;
struct Compass_vector;

/* Compass_position */
// Compass_position describes a position in terms of bearing and range
struct Compass_position
{
    double bearing;
    double range;       

    Compass_position (double in_bearing = 0., double in_range = 0.) : 
        bearing(in_bearing), range(in_range)
        {}

    // construct a Compass_position from two Points, giving
    // bearing and range of p2 from p1.
    Compass_position(const Point& p1, const Point& p2);

    // construct a Compass_position from a Polar_vector
    Compass_position (const Polar_vector& pv);
};

/* Course_speed */
// Course_speed describes a compass course and speed. 
// A Course_speed can not be constructed from any other object.
struct Course_speed
{
    double course;
    double speed;       

    Course_speed (double in_course = 0., double in_speed = 0.) : 
            course(in_course), speed(in_speed)
            {}
};


/* Compass_vector */
// Compass_vector describes a displacement in 
// terms of compass direction and distance
struct Compass_vector
{
    double direction;
    double distance;    

    Compass_vector (double in_direction = 0., double in_distance = 0.) : 
        direction(in_direction), distance(in_distance)
        {}

    Compass_vector (const Polar_vector& pv);

    // construct a Compass_vector from two Points, giving 
    // the vector for moving from p1 to p2.
    Compass_vector(const Point& p1, const Point& p2);

};

// *** Overloaded operator prototypes ***
// Operators are all defined as non-member 
// functions for simplicity in documentation

// Adding a Point and a Compass_position yields a Point
Point operator+ (const Point& p, const Compass_position& cp);
Point operator+ (const Compass_position& cp, const Point& p);

// Adding a Point and a Compass_vector yields a Point
Point operator+ (const Point& p, const Compass_vector& cv);
Point operator+ (const Compass_vector& cv, const Point& p);

// Multiplying a Course_speed by a double yields a Compass_vector
Compass_vector operator* (const Course_speed& cs, double d);
Compass_vector operator* (double d, const Course_speed& cs);

// Output operator overloads
std::ostream& operator<< (std::ostream& os, const Course_speed& p);
std::ostream& operator<< (std::ostream& os, const Compass_position& cp);
std::ostream& operator<< (std::ostream& os, const Compass_vector& cv);

// *** Other navigation functions  ***

// Given ownship's course and speed, and
// the target's course and speed, and bearing and range from ownship,
// compute the range and bearing of the point
// of closest approach and the time until the point.
// If the CPA is the current position,
//it is returned with the time being zero.
Compass_position compute_CPA(Course_speed ownship_cs,
                             Course_speed target_cs,
                             Compass_position target_position_cp,
                             double& time_to_CPA);

#endif
