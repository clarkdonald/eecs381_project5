#include "Track_base.h"
#include "Navigation.h"

#include <iostream>
#include <cmath>

using namespace std;

/* Public Function Definitions */

Track_base::Track_base() : altitude(0.)
{
    std::cout << "Track_base constructed" << std::endl;
}

Track_base::Track_base(Point in_position) : position(in_position), altitude(0.)
{
    std::cout << "Track_base constructed" << std::endl;
}

Track_base::Track_base(Point in_position,
                       Course_speed in_course_speed,
                       double in_altitude) :
    position(in_position),
    course_speed(in_course_speed),
    altitude(in_altitude)
{
    std::cout << "Track_base constructed" << std::endl;
}

Track_base::~Track_base()
{
    std::cout << "Track_base destructed" << std::endl;
}

// range and bearing of this track from a specified position
Compass_position Track_base::get_range_and_bearing_from (const Point& p) const
{
    Compass_position result(p, position);
    return result;
}

// range and bearing of this track from a specified track
Compass_position Track_base::get_range_and_bearing_from (const Track_base * track_ptr) const
{
    Compass_position result(track_ptr->position, position);
    return result;
}

// range and bearing of closet point of approach from a specified track
// the other track is "ownship" normally, so this appears to be "backwards"
Compass_position Track_base::get_CPA_range_and_bearing_from(const Track_base *track_ptr) const
{
    double time_to_CPA;
    Compass_position result = 
            compute_CPA(track_ptr->get_course_speed(),
                        course_speed,
                        get_range_and_bearing_from(track_ptr),
                        time_to_CPA);
    return result;
}

// calculate whether this track is inbound towards a specified track
bool Track_base::is_inbound_toward(const Track_base * track_ptr) const
{
    bool result = false;
    // is this correct if course/bearing on different sides?
    // get reciprocal of bearing from this track
    Compass_position other_position = get_range_and_bearing_from(track_ptr);

    // is our course within 90 degrees of reciprocal of other's bearing?
    double reciprocal_bearing = fmod(other_position.bearing + 180., 360.);

    double diff = fabs(course_speed.course - reciprocal_bearing);
    diff = (diff > 180) ? (360 - diff) : diff; // normalize to smallest angle
    if (diff < 90.)
        result = true;
    else
        result = false;

    return result;
}

// update the position of this object
void Track_base::update_position(double time_increment)
{
    position = position + (course_speed * time_increment);
}

