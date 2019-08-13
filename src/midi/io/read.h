//
// Created by indy on 30/04/19.
//

#ifndef MIDI_PROJECT_READ_H
#define MIDI_PROJECT_READ_H

#include <istream>
#include <memory>
#include "logging.h"

namespace io
{
    template<typename T>
    void read_to(std::istream& istream,T* buffer,size_t size = 1)
    {
        istream.read(reinterpret_cast<char*>(buffer), (sizeof(T) * size));
        CHECK(!istream.fail()) << "Failed reading to input stream!";
    }

    template<typename T, typename std::enable_if<std::is_fundamental<T>::value, T>::type* = nullptr>
    T read(std::istream& istream)
    {
        T result;
        read_to(istream, &result);

        return result;
    }

    template<typename T>
    std::unique_ptr<T[]> read_array(std::istream& istream,size_t size)
    {
        auto array = std::make_unique<T[]>(size);
        read_to(istream, array.get(), size);

        return std::move(array);
    }
}

#endif //MIDI_PROJECT_READ_H
