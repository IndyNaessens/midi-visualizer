//
// Created by indy on 30/04/19.
//

#include "vli.h"
#include "read.h"

uint64_t io::read_variable_length_integer(std::istream& istream)
{
    uint64_t result = 0;
    while(true)
    {
        auto byte = read<uint8_t>(istream);

        uint8_t data = byte & 127U;
        result = (result << 7U) | data;

        if(byte >> 7U == 0) return result;
    }
}