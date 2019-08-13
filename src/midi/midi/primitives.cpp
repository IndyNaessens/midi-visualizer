//
// Created by indy on 1/05/19.
//

#include "primitives.h"

midi::Duration midi::operator+(const midi::Duration &duration_l, const midi::Duration &duration_r)
{
    return midi::Duration(value(duration_l) + value(duration_r));
}

midi::Time midi::operator+(const midi::Time &time, const midi::Duration &duration)
{
    return midi::Time(value(time) + value(duration));
}

midi::Time midi::operator+(const midi::Duration &duration, const midi::Time &time)
{
    return midi::Time(value(duration) + value(time));
}

midi::Duration midi::operator-(const midi::Time &time_l, const midi::Time &time_r)
{
    return midi::Duration(value(time_l) - value(time_r));
}

midi::Duration midi::operator-(const midi::Duration &duration_l, const midi::Duration &duration_r)
{
    return midi::Duration(value(duration_l) - value(duration_r));
}

midi::Time& midi::operator+=(midi::Time &time, const midi::Duration &duration)
{
    return (time = time + duration);
}

midi::Duration& midi::operator+=(midi::Duration &duration_l, const midi::Duration &duration_r)
{
    return (duration_l = duration_l + duration_r);
}

midi::Duration& midi::operator-=(midi::Duration &duration_l, const midi::Duration &duration_r)
{
    return (duration_l = duration_l - duration_r);
}
