//
// Created by indy on 19/03/19.
//

#ifndef MIDI_PROJECT_ENDIANNESS_H
#define MIDI_PROJECT_ENDIANNESS_H

#include <cstdint>

namespace io
{
    void switch_endianness(uint16_t*);
    void switch_endianness(uint32_t*);
    void switch_endianness(uint64_t*);
}

#endif //MIDI_PROJECT_ENDIANNESS_H
