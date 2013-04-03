/*
Navigation.cpp implementation file.
See Navigation.h for comments
*/

#include "Navigation.h"
#include "Geometry.h"
#include <iostream>
#include <cmath>
#include <cassert>

using namespace std;

// Compass degrees and trig degrees are backwards from each other
// and are offset by 90 degrees: 
// 0 trig = 90 compass, 0 compass = 90 trig
// 180 trig = 270 compass, 180 compass = 270 trig
// input and is assumed to be positive, and output will be positive
// this function should never produce an output of 360 instead of zero
/*
 45.00 -> 45.00
  0.00 -> 90.00
 90.00 ->  0.00
360.00 -> 90.00
*/
inline double to_other_degrees(double deg_in)
{       
    return fmod(360. + 90. - deg_in, 360.);
}


inline Polar_vector to_Polar_vector(Compass_vector cv)
{
    return Polar_vector(cv.distance, to_radians(to_other_degrees(cv.direction)));
}

inline Polar_vector to_Polar_vector(Compass_position cp)
{
    return Polar_vector(cp.range, to_radians(to_other_degrees(cp.bearing)));
}

// *** Compass_position members ***
Compass_position::Compass_position (const Polar_vector& pv)
{
    bearing = to_other_degrees(to_degrees(pv.theta));
    range = pv.r;
}

// construct a Compass_position from two Points, giving
// bearing and range of p2 from p1.
Compass_position::Compass_position(const Point& p1, const Point& p2)
{
    Polar_vector pv (p1, p2);
    bearing = to_other_degrees(to_degrees(pv.theta));
    range = pv.r;
}
        

// *** Compass_vector members ***


// construct a Compass_vector from two positions, giving 
// the vector for moving from p1 to p2.
Compass_vector::Compass_vector(const Point& p1, const Point& p2)
{
    Polar_vector pv (p1, p2);
    direction = to_other_degrees(to_degrees(pv.theta));
    distance = pv.r;
}

Compass_vector::Compass_vector (const Polar_vector& pv)
{
    direction = to_other_degrees(to_degrees(pv.theta));
    distance = pv.r;
}


// *** Overloaded operators ***
// Operators are all defined as non-member functions for simplicity in documentation

// Adding a Point and a Compass_position yields a Point
Point operator+ (const Point& p, const Compass_position& cp)
{
    Point pn = Point(p + to_Polar_vector(cp));
    return pn;
}
        
Point operator+ (const Compass_position& cp, const Point& p)
{
    return p + cp;
}

// Adding a Point and a Compass_vector yields a Point
Point operator+ (const Point& p, const Compass_vector& cv)
{
    Point pn = Point(p + to_Polar_vector(cv));
    return pn;
}

Point operator+ (const Compass_vector& cv, const Point& p)
{
    return p + cv;
}


// Multiplying a Course_speed by a double yields a Compass_vector
// with same angle but scaled distance
Compass_vector operator* (const Course_speed& cs, double d)
{
    return Compass_vector(cs.course, cs.speed * d);
}

Compass_vector operator* (double d, const Course_speed& cs)
{
    return cs * d;
}

// Output operator overloads

// output a Course_speed as "course deg, speed nm/hr"
ostream& operator<< (ostream& os, const Course_speed& cs)
{
    // if course will round to 360.00 in the output (2 decimal places),
    // alter the output value to be 0.00; leave the actual direction alone.
    // The true value of direction is assumed to be always less than exactly 360.

    assert(cs.course < 360.);   // catch a programming error

    double output_course = cs.course;

    if ((output_course + .005) >= 360.)
        output_course = 0.00;

    os << "course " << output_course << " deg, speed " << cs.speed << " nm/hr";
    return os;
}

// output a Compass_position as "bearing deg, range nm"
ostream& operator<< (ostream& os, const Compass_position& cp)
{
    // if bearing will round to 360.00 in the output (2 decimal places),
    // alter the output value to be 0.00; leave the actual bearing alone.
    // The true value of bearing is assumed to be always less than exactly 360.

    assert(cp.bearing < 360.);  // catch a programming error

    double output_bearing = cp.bearing;

    if ((output_bearing + .005) >= 360.)
        output_bearing = 0.00;

    os << "bearing " << output_bearing << " deg, range " << cp.range << " nm";
    return os;
}

// output a Compass_vector as "direction deg, distance nm"
ostream& operator<< (ostream& os, const Compass_vector& cv)
{
    // if direction will round to 360.00 in the output (2 decimal places),
    // alter the output value to be 0.00; leave the actual direction alone.
    // The true value of direction is assumed to be always less than exactly 360.

    assert(cv.direction < 360.);    // catch a programming error

    double output_direction = cv.direction;

    if ((output_direction + .005) >= 360.)
        output_direction = 0.00;

    os << "direction " << output_direction << " deg, distance " << cv.distance << " nm";
    return os;
}

// *** Other navigation functions  ***

// *** compute_CPA ***
// Given ownship's course and speed, and the target's
// course and speed, and bearing and range from ownship,
// compute the range and bearing of the point of closest
// approach and the time until the point.
// If the CPA is the current position, it is returned with the time being zero.
// The algorithm used is based on code written by Al Gerheim.

Compass_position compute_CPA(Course_speed ownship_cs,
                             Course_speed target_cs,
                             Compass_position target_position_cp,
                             double& time_to_CPA)
{

    // convert the two courses and speeds to Cartesian vectors
    Cartesian_vector ownship_cv(Polar_vector(ownship_cs.speed, to_radians(to_other_degrees (ownship_cs.course))));
    Cartesian_vector target_cv(Polar_vector(target_cs.speed, to_radians(to_other_degrees (target_cs.course))));
    // compute a vector for the position of the target right now (time = 0) relative to ownship.
    Cartesian_vector relative_target_position(to_Polar_vector(target_position_cp));

    // compute a vector that describes the target's motion relative to ownship,
    // which is effectively at (0, 0).
    Cartesian_vector relative_target_motion = target_cv - ownship_cv;

    // compute parameter along relative motion line corresponding to closest point - note similarity to 
    // distance-from-line computations - this is the time of closest approach.
    // If relative distance is decreasing, this time will be negative.

    double t = (relative_target_motion.delta_x * relative_target_position.delta_x + 
                relative_target_motion.delta_y * relative_target_position.delta_y) 
                    /
                (relative_target_motion.delta_x * relative_target_motion.delta_x +
                 relative_target_motion.delta_y * relative_target_motion.delta_y);

    // if t is greater than 0, it means closest point was the initial point, at time = 0.
    if (t > 0.)
    {
        time_to_CPA = 0.;
        return target_position_cp;
    }
    else
    {
        time_to_CPA = fabs(t);
        // advance the relative motion vector to time t in the future
        Cartesian_vector future_target_displacement = time_to_CPA * relative_target_motion;
        return Compass_position(Polar_vector(relative_target_position + future_target_displacement));
    }
}



