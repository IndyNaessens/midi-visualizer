//
// Created by indy on 30/04/19.
//

#ifndef MIDI_PROJECT_VLI_H
#define MIDI_PROJECT_VLI_H

#include <cstdint>
#include <istream>

namespace io
{
    uint64_t read_variable_length_integer(std::istream&);
}

#endif //MIDI_PROJECT_VLI_H
