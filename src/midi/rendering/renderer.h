//
// Created by indy on 24/05/19.
//

#ifndef MIDI_PROJECT_RENDERER_H
#define MIDI_PROJECT_RENDERER_H

#include "../imaging/bitmap.h"
#include "../imaging/bmp-format.h"
#include "../imaging/color.h"
#include "../midi/midi.h"
#include "../util/position.h"
#include <memory>

namespace rendering {

    struct NOTE_RENDERING_DATA
    {
        unsigned note_height;
        uint8_t lowest_note_number_value;
        uint8_t highest_note_number_value;
        unsigned ending_note_time_value;

        NOTE_RENDERING_DATA(unsigned note_height, unsigned lowest_note_number_value, unsigned highest_note_number_value, unsigned ending_note_time_value):
                note_height(note_height), lowest_note_number_value(lowest_note_number_value), highest_note_number_value(highest_note_number_value), ending_note_time_value(ending_note_time_value) {};
    };

    class Renderer
    {

        std::unique_ptr<imaging::Bitmap> bitmap;
        std::unique_ptr<NOTE_RENDERING_DATA> note_rendering_data;
        unsigned frame_width;
        unsigned horizontal_step;
        unsigned horizontal_scale;

        Position transform_note(const midi::NOTE &note) const;
        unsigned calculate_note_width(const midi::NOTE &note) const;

    public:
        Renderer(unsigned frame_width, unsigned horizontal_step, unsigned horizontal_scale, const NOTE_RENDERING_DATA& note_rendering_data);

        void draw_note(const midi::NOTE &note);
        void render_frames(const std::string& target_directory_path, const std::string& pattern) const;
    };
}


#endif //MIDI_PROJECT_RENDERER_H
