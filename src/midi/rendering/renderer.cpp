//
// Created by indy on 24/05/19.
//

#include "renderer.h"
#include <iomanip>
#include <new>

using namespace rendering;

Renderer::Renderer(unsigned frame_width, unsigned horizontal_step, unsigned horizontal_scale, const NOTE_RENDERING_DATA& note_rendering_data)
        : frame_width(frame_width),horizontal_step(horizontal_step),horizontal_scale(horizontal_scale),note_rendering_data(std::make_unique<NOTE_RENDERING_DATA>(note_rendering_data))
{
    try
    {
        this->bitmap = std::make_unique<imaging::Bitmap>((note_rendering_data.ending_note_time_value/20) * horizontal_scale,
                                                         (note_rendering_data.note_height*(note_rendering_data.highest_note_number_value - note_rendering_data.lowest_note_number_value + 1)));
    }catch(std::bad_alloc&)
    {
        std::cout << "\nThe bitmap is scaled too big, falling back to horizontal scale 1!\n";
        this->horizontal_scale = 1;
        this->bitmap = std::make_unique<imaging::Bitmap>((note_rendering_data.ending_note_time_value/20),
                                                         (note_rendering_data.note_height*(note_rendering_data.highest_note_number_value - note_rendering_data.lowest_note_number_value + 1)));
    }
}

void Renderer::draw_note(const midi::NOTE& note)
{
    auto position = transform_note(note);

    for(unsigned i=0; i != note_rendering_data->note_height; ++i)
    {
        for(unsigned j=0; j != calculate_note_width(note); ++j)
        {
            (*bitmap)[position + Position(j,i)] = imaging::Color {
                    static_cast<double>(value(note.note_number)),
                    static_cast<double >(value(note.instrument)),
                    static_cast<double >(note.velocity)
            };
        }
    }
}

void Renderer::render_frames(const std::string& target_directory_path, const std::string& pattern) const
{
    std::stringstream string_stream;
    if(frame_width == 0)
    {
        string_stream << std::setfill('0') << std::setw(5) << (0);
        auto frame_name = pattern;

        imaging::save_as_bmp(target_directory_path + frame_name.replace(frame_name.find("%d"),2,string_stream.str()) + ".bmp", *bitmap);
    }
    else
    {
        for(unsigned i=0;i <= bitmap->width() - frame_width; i+=horizontal_step)
        {
            auto sliced_bitmap = bitmap->slice(static_cast<int>(i), 0, static_cast<int>(frame_width), static_cast<int>(bitmap->height()));

            string_stream << std::setfill('0') << std::setw(5) << (i/horizontal_step);
            auto frame_name = pattern;

            imaging::save_as_bmp(target_directory_path + frame_name.replace(frame_name.find("%d"),2,string_stream.str()) + ".bmp",*sliced_bitmap);
            string_stream.str(std::string());
        }
    }
}

Position Renderer::transform_note(const midi::NOTE& note) const
{
    return Position(((value(note.start)/20) * horizontal_scale),(note_rendering_data->highest_note_number_value - value(note.note_number)) * note_rendering_data->note_height);
}

unsigned Renderer::calculate_note_width(const midi::NOTE& note) const
{
    return ((value(note.duration)/20) * horizontal_scale);
}
