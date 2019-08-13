#ifndef TEST_BUILD

#include "rendering/renderer.h"
#include <fstream>
#include "midi/midi.h"
#include <algorithm>
#include "shell/command-line-parser.h"

int main(int argc, char** argv)
{
    //params
    unsigned frame_width = 0;
    unsigned note_height = 16;
    unsigned horizontal_step = 1;
    unsigned horizontal_scale = 1;
    std::string file_path;
    std::string pattern;

    //read command line arguments
    shell::CommandLineParser parser;

    parser.add_argument("-w", &frame_width);
    parser.add_argument("-d", &horizontal_step);
    parser.add_argument("-s", &horizontal_scale);
    parser.add_argument("-h", &note_height);
    parser.process(argc, argv);

    if(parser.positional_arguments().size() < 2)
    {
        std::cout << "\nPlease provide all needed arguments!";
        exit(EXIT_FAILURE);
    }

    file_path = parser.positional_arguments()[0];
    pattern = parser.positional_arguments()[1];

    //open file
    std::ifstream input_file_stream(file_path, std::ios_base::binary);

    //read the notes
    const auto notes = midi::read_notes(input_file_stream);

    //calculate the width needed for the renderer
    const auto ending_note = std::max_element(notes.begin(),notes.end(),
            [](const midi::NOTE& note_l, const midi::NOTE& note_r)
            {
                return (value(note_l.start) + value(note_l.duration)) < (value(note_r.start) + value(note_r.duration));
            });

    //get the lowest and highest note
    const auto [lowest_note,highest_note] = std::minmax_element(notes.begin(),notes.end(),
            [](const midi::NOTE& note_l, const midi::NOTE& note_r)
            {
                return value(note_l.note_number) < value(note_r.note_number);
            });

    //init renderer
    const auto note_rendering_data = rendering::NOTE_RENDERING_DATA(note_height,value(lowest_note->note_number), value(highest_note->note_number), value(ending_note->start + ending_note->duration));
    auto renderer = rendering::Renderer(frame_width,horizontal_step,horizontal_scale, note_rendering_data);

    for(auto& note: notes) {
        renderer.draw_note(note);
    }

    renderer.render_frames("/home/indy/Documents/midi-finished-build/frames/", pattern);
}

#endif