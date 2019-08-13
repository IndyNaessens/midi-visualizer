//
// Created by indy on 1/05/19.
//

#ifndef MIDI_PROJECT_PRIMITIVES_H
#define MIDI_PROJECT_PRIMITIVES_H

#include "util/tagged.h"
#include <cstdint>

namespace midi
{
    struct Channel : tagged<uint8_t,Channel>, equality<Channel>, show_value<Channel, int> { using tagged::tagged; };
    struct Instrument : tagged<uint8_t, Instrument>, equality<Instrument>, show_value<Instrument, int> { using tagged::tagged; };
    struct NoteNumber : tagged<uint8_t, NoteNumber>, show_value<NoteNumber, int>, ordered<NoteNumber> { using tagged::tagged; };
    struct Time : tagged<uint64_t , Time>, ordered<Time>, show_value<Time, int> { using tagged::tagged; };
    struct Duration : tagged<uint64_t , Duration>, ordered<Duration>, show_value<Duration, int> { using tagged::tagged; };

    Duration operator +(const Duration&, const Duration&);
    Time operator +(const Time&, const Duration&);
    Time operator +(const Duration&, const Time&);
    Duration operator -(const Time&, const Time&);
    Duration operator -(const Duration&, const Duration&);

    Time& operator +=(Time&, const Duration&);
    Duration& operator +=(Duration&, const Duration&);
    Duration& operator -=(Duration&, const Duration&);
}
#endif //MIDI_PROJECT_PRIMITIVES_H
