//
// Created by indy on 1/05/19.
//

#include "midi.h"
#include "io/read.h"
#include "io/endianness.h"
#include "io/vli.h"
#include <string>

//CHUNK_HEADER
void midi::read_chunk_header(std::istream& istream, midi::CHUNK_HEADER* chunk_header)
{
    io::read_to(istream,chunk_header);
    io::switch_endianness(&chunk_header->size);
}

std::string midi::header_id(const midi::CHUNK_HEADER& chunk_header)
{
    return std::string(chunk_header.id,4);
}
//END CHUNK_HEADER

//MTHD
void midi::read_mthd(std::istream& istream, midi::MTHD* mthd)
{
    io::read_to(istream,mthd);
    io::switch_endianness(&mthd->header.size);
    io::switch_endianness(&mthd->type);
    io::switch_endianness(&mthd->ntracks);
    io::switch_endianness(&mthd->division);
}
//END MTHD

//MTRK
bool midi::is_sysex_event(uint8_t event_identifier)
{
    return event_identifier == 0xF0 || event_identifier == 0xF7;
}

bool midi::is_meta_event(uint8_t event_identifier)
{
    return event_identifier == 0xFF;
}

bool midi::is_running_status(uint8_t event_identifier)
{
    //shift bits 7 times to the right 1000 0000 becomes 1
    return (event_identifier >> 7U) == 0;
}

//midi event is 1 byte (0xkn)
//lower 4 bits (n) => channel (0-15)
//upper 4 bits (k) => type (8,9,A,B,C,D,E)
bool midi::is_midi_event(uint8_t event_identifier)
{
    uint8_t type = extract_midi_event_type(event_identifier);
    Channel channel = extract_midi_event_channel(event_identifier);

    return (type >= 0x8 && type <= 0xE) && (value(channel) >= 0x0 && value(channel) <= 0xF);
}

uint8_t midi::extract_midi_event_type(uint8_t midi_event_status)
{
    //shift bits 4 times to the right, get upper 4 bits, example
    //1000 1111 becomes 1000
    return midi_event_status >> 4U;
}

midi::Channel midi::extract_midi_event_channel(uint8_t midi_event_status)
{
    //bitwise AND operator, get lower 4 bits, example
    //1000 1111
    //           AND
    //0000 1111
    //0000 1111 => result
    return midi::Channel(midi_event_status & 0x0FU);
}

bool midi::is_note_off(uint8_t midi_event_status_type)
{
    return midi_event_status_type == 0x08;
}

bool midi::is_note_on(uint8_t midi_event_status_type)
{
    return midi_event_status_type == 0x09;
}

bool midi::is_polyphonic_key_pressure(uint8_t midi_event_status_type)
{
    return midi_event_status_type == 0x0A;
}

bool midi::is_control_change(uint8_t midi_event_status_type)
{
    return midi_event_status_type == 0x0B;
}

bool midi::is_program_change(uint8_t midi_event_status_type)
{
    return midi_event_status_type == 0x0C;
}

bool midi::is_channel_pressure(uint8_t midi_event_status_type)
{
    return midi_event_status_type == 0x0D;
}

bool midi::is_pitch_wheel_change(uint8_t midi_event_status_type)
{
    return midi_event_status_type == 0x0E;
}

bool midi::is_end_of_track_event(uint8_t meta_event_type)
{
    return meta_event_type == 0x2F;
}

void midi::read_mtrk(std::istream& istream, midi::EventReceiver& event_receiver)
{
    //read mtrk header
    midi::CHUNK_HEADER mtrk_header;
    io::read_to(istream,&mtrk_header);
    io::switch_endianness(&mtrk_header.size);

    //sanity check
    CHECK(header_id(mtrk_header) == "MTrk") << "Not a valid mtrk";

    bool end_of_track_reached = false;
    uint8_t id(0);
    while(!end_of_track_reached)
    {
        //get delta time
        auto dt = io::read_variable_length_integer(istream);

        //get possible event identifier also called status when midi event, if the status is running the status is omitted
        if(!is_running_status(istream.peek())) id = io::read<uint8_t >(istream);

        if(is_meta_event(id))
        {
            auto type = io::read<uint8_t>(istream);
            auto length = io::read_variable_length_integer(istream);
            auto data = io::read_array<uint8_t >(istream,length);

            event_receiver.meta(midi::Duration(dt),type,std::move(data),length);
            if(is_end_of_track_event(type)) end_of_track_reached = true;
        }
        else if(is_sysex_event(id))
        {
            auto length = io::read_variable_length_integer(istream);
            auto data = io::read_array<uint8_t >(istream,length);

            event_receiver.sysex(midi::Duration(dt),std::move(data),length);
        }
        else if(is_midi_event(id))
        {
            auto midi_event_type = extract_midi_event_type(id);
            auto midi_event_channel = extract_midi_event_channel(id);

            if(is_note_off(midi_event_type))
            {
                auto note = midi::NoteNumber(io::read<uint8_t >(istream));
                auto velocity = io::read<uint8_t >(istream);

                event_receiver.note_off(midi::Duration(dt),midi_event_channel,note,velocity);
            }
            else if(is_note_on(midi_event_type))
            {
                auto note = midi::NoteNumber(io::read<uint8_t >(istream));
                auto velocity = io::read<uint8_t >(istream);

                event_receiver.note_on(midi::Duration(dt),midi_event_channel,note,velocity);
            }
            else if(is_polyphonic_key_pressure(midi_event_type))
            {
                auto note = midi::NoteNumber(io::read<uint8_t >(istream));
                auto pressure = io::read<uint8_t >(istream);

                event_receiver.polyphonic_key_pressure(midi::Duration(dt),midi_event_channel,note,pressure);
            }
            else if(is_control_change(midi_event_type))
            {
                auto controller = io::read<uint8_t >(istream);
                auto value = io::read<uint8_t >(istream);

                event_receiver.control_change(midi::Duration(dt),midi_event_channel,controller,value);
            }
            else if(is_program_change(midi_event_type))
            {
                auto program = midi::Instrument(io::read<uint8_t >(istream));

                event_receiver.program_change(midi::Duration(dt),midi_event_channel,program);
            }
            else if(is_channel_pressure(midi_event_type))
            {
                auto pressure = io::read<uint8_t >(istream);

                event_receiver.channel_pressure(midi::Duration(dt),midi_event_channel,pressure);
            }
            else if(is_pitch_wheel_change(midi_event_type))
            {
                //our value is 14055 or 0b11011011100111
                auto lower_bits = io::read<uint8_t>(istream); //we have 01100111
                auto upper_bits = io::read<uint8_t>(istream); //we have 01101101

                //16 bits 000000000 00000000
                uint16_t position = upper_bits << 7u; //shift upper bits 7 times to the right => 00110110 10000000
                position = position | lower_bits; //bitwise or operator with the lower bits
                //00110110 10000000 OR
                //00000000 01100111 => 00110110 11100111, we have our value!

                event_receiver.pitch_wheel_change(midi::Duration(dt),midi_event_channel, position);
            }
        }
    }
}
//END MTRK

//NOTE
bool midi::operator==(const midi::NOTE& note_l,const midi::NOTE& note_r)
{
    return (note_l.note_number == note_r.note_number) &&
           (note_l.instrument == note_r.instrument) &&
           (note_l.duration == note_r.duration) &&
           (note_l.start == note_r.start) &&
           (note_l.velocity == note_r.velocity);
}

bool midi::operator!=(const midi::NOTE& note_l,const midi::NOTE& note_r)
{
    return !(note_l == note_r);
}

std::ostream& midi::operator<<(std::ostream& out, const NOTE& note)
{
    return out << "Note(number=" << note.note_number
               << ",start=" << note.start
               << ",duration=" << note.duration
               << ",instrument=" << note.instrument << ")";
}

midi::Duration midi::calculate_note_duration(const midi::Time& start_time, const midi::Time& end_time)
{
    return end_time - start_time;
}
//END NOTE

//CHANNEL NOTE COLLECTOR
void midi::ChannelNoteCollector::note_on(midi::Duration dt, midi::Channel channel, midi::NoteNumber note, uint8_t velocity)
{
    increase_current_time(dt);
    if(channel != current_channel) return;

    if(velocity == 0) //handle note off event
    {
        note_off(Duration(0),channel,note,velocity);
    }
    else if(std::any_of(started_notes.begin(),started_notes.end(),[&note](const NOTE& note_on){ return note_on.note_number == note; }))   //double note on event(1,2) => note on(1) note off(1) note on(2)
    {
        note_off(Duration(0),channel,note,velocity);
        save_note_on(NOTE(note,current_time,Duration(0),velocity,current_instrument));
    }
    else //normal note on event
    {
        save_note_on(NOTE(note,current_time,Duration(0),velocity,current_instrument));
    }
}

void midi::ChannelNoteCollector::note_off(midi::Duration dt, midi::Channel channel, midi::NoteNumber note,uint8_t velocity)
{
    increase_current_time(dt);
    if(channel != current_channel) return;

    auto found_note_it = std::find_if(started_notes.begin(),started_notes.end(),[&note](const NOTE& note_on){ return note_on.note_number == note; });
    if(found_note_it != started_notes.end())
    {
        //calculate duration and call function
        found_note_it->duration = calculate_note_duration(found_note_it->start,current_time);
        note_receiver(*found_note_it);

        //remove the note because it's turned off
        started_notes.erase(found_note_it);
    }
}

void midi::ChannelNoteCollector::polyphonic_key_pressure(midi::Duration dt, midi::Channel channel, midi::NoteNumber note, uint8_t pressure)
{
    increase_current_time(dt);
}

void midi::ChannelNoteCollector::control_change(midi::Duration dt, midi::Channel channel, uint8_t controller, uint8_t value)
{
    increase_current_time(dt);
}

void midi::ChannelNoteCollector::program_change(midi::Duration dt, midi::Channel channel, midi::Instrument program)
{
    increase_current_time(dt);
    if(channel != current_channel) return;

    this->current_instrument = program;
}

void midi::ChannelNoteCollector::channel_pressure(midi::Duration dt, midi::Channel channel, uint8_t pressure)
{
    increase_current_time(dt);
}

void midi::ChannelNoteCollector::pitch_wheel_change(midi::Duration dt, midi::Channel channel, uint16_t value)
{
    increase_current_time(dt);
}

void midi::ChannelNoteCollector::meta(midi::Duration dt, uint8_t type, std::unique_ptr<uint8_t[]> data, uint64_t data_size)
{
    if(is_end_of_track_event(type))  //end of track so prepare new track
    {
        new_track();
    } else increase_current_time(dt);
}

void midi::ChannelNoteCollector::sysex(midi::Duration dt, std::unique_ptr<uint8_t[]> data, uint64_t data_size)
{
    increase_current_time(dt);
}

void midi::ChannelNoteCollector::increase_current_time(const Duration& delta_time)
{
    this->current_time += delta_time;
}

void midi::ChannelNoteCollector::save_note_on(const midi::NOTE& note)
{
    this->started_notes.push_back(note);
}

void midi::ChannelNoteCollector::new_track()
{
    current_time = Time(0);
    current_instrument = Instrument(0);
}
//END CHANNEL NOTE COLLECTOR

//EVENT MULTICASTER
void midi::EventMulticaster::note_on(midi::Duration dt, midi::Channel channel, midi::NoteNumber note, uint8_t velocity)
{
    for(const auto& event_receiver: event_receivers)
    {
        event_receiver->note_on(dt,channel,note,velocity);
    }
}

void midi::EventMulticaster::note_off(midi::Duration dt, midi::Channel channel, midi::NoteNumber note, uint8_t velocity)
{
    for(const auto& event_receiver: event_receivers)
    {
        event_receiver->note_off(dt,channel,note,velocity);
    }
}

void midi::EventMulticaster::polyphonic_key_pressure(midi::Duration dt, midi::Channel channel, midi::NoteNumber note, uint8_t pressure)
{
    for(const auto& event_receiver: event_receivers)
    {
        event_receiver->polyphonic_key_pressure(dt,channel,note,pressure);
    }
}

void midi::EventMulticaster::control_change(midi::Duration dt, midi::Channel channel, uint8_t controller, uint8_t value)
{
    for(const auto& event_receiver: event_receivers)
    {
        event_receiver->control_change(dt,channel,controller,value);
    }
}

void midi::EventMulticaster::program_change(midi::Duration dt, midi::Channel channel, midi::Instrument program)
{
    for(const auto& event_receiver: event_receivers)
    {
        event_receiver->program_change(dt,channel,program);
    }
}

void midi::EventMulticaster::channel_pressure(midi::Duration dt, midi::Channel channel, uint8_t pressure)
{
    for(const auto& event_receiver: event_receivers)
    {
        event_receiver->channel_pressure(dt,channel,pressure);
    }
}

void midi::EventMulticaster::pitch_wheel_change(midi::Duration dt, midi::Channel channel, uint16_t value)
{
    for(const auto& event_receiver: event_receivers)
    {
        event_receiver->pitch_wheel_change(dt,channel,value);
    }
}

void midi::EventMulticaster::meta(midi::Duration dt, uint8_t type, std::unique_ptr<uint8_t[]> data, uint64_t data_size)
{
    for(const auto& event_receiver: event_receivers)
    {
        event_receiver->meta(dt,type,std::move(data),data_size);
    }
}

void midi::EventMulticaster::sysex(midi::Duration dt, std::unique_ptr<uint8_t[]> data, uint64_t data_size)
{
    for(const auto& event_receiver: event_receivers)
    {
        event_receiver->sysex(dt,std::move(data),data_size);
    }
}

void midi::EventMulticaster::add_event_receiver(const std::shared_ptr<EventReceiver>& eventReceiver)
{
    if(event_receivers.size() < 16)
    {
        event_receivers.push_back(eventReceiver);
    }
}
//END EVENT MULTICASTER

//NOTE COLLECTOR
midi::NoteCollector::NoteCollector(std::function<void(const midi::NOTE &)> function)
        : event_multicaster(midi::EventMulticaster(std::vector<std::shared_ptr<midi::EventReceiver>>()))
{
    for(int i=0; i!=16; ++i)
    {
        auto note_channel_collector = std::make_shared<ChannelNoteCollector>(Channel(i),function);
        event_multicaster.add_event_receiver(note_channel_collector);
    }
}

void midi::NoteCollector::note_on(midi::Duration dt, midi::Channel channel, midi::NoteNumber note, uint8_t velocity)
{
    event_multicaster.note_on(dt,channel,note,velocity);
}

void midi::NoteCollector::note_off(midi::Duration dt, midi::Channel channel, midi::NoteNumber note, uint8_t velocity)
{
    event_multicaster.note_off(dt,channel,note,velocity);
}

void midi::NoteCollector::polyphonic_key_pressure(midi::Duration dt, midi::Channel channel, midi::NoteNumber note, uint8_t pressure)
{
    event_multicaster.polyphonic_key_pressure(dt,channel,note,pressure);
}

void midi::NoteCollector::control_change(midi::Duration dt, midi::Channel channel, uint8_t controller, uint8_t value)
{
    event_multicaster.control_change(dt,channel,controller,value);
}

void midi::NoteCollector::program_change(midi::Duration dt, midi::Channel channel, midi::Instrument program)
{
    event_multicaster.program_change(dt,channel,program);
}

void midi::NoteCollector::channel_pressure(midi::Duration dt, midi::Channel channel, uint8_t pressure)
{
    event_multicaster.channel_pressure(dt,channel,pressure);
}

void midi::NoteCollector::pitch_wheel_change(midi::Duration dt, midi::Channel channel, uint16_t value)
{
    event_multicaster.pitch_wheel_change(dt,channel,value);
}

void midi::NoteCollector::meta(midi::Duration dt, uint8_t type, std::unique_ptr<uint8_t[]> data, uint64_t data_size)
{
    event_multicaster.meta(dt,type,std::move(data),data_size);
}

void midi::NoteCollector::sysex(midi::Duration dt, std::unique_ptr<uint8_t[]> data, uint64_t data_size)
{
    event_multicaster.sysex(dt,std::move(data),data_size);
}
//END NOTE COLLECTOR

std::vector<midi::NOTE> midi::read_notes(std::istream& istream)
{
    std::vector<NOTE> notes;

    //read mthd
    MTHD mthd;
    read_mthd(istream,&mthd);

    CHECK(mthd.type != 2) << "MTHD with type 2 is not supported";

    //our event receiver
    NoteCollector noteCollector([&notes](const midi::NOTE& note) { notes.push_back(note); });

    //read mtrk's
    for(int i=0;i<mthd.ntracks;++i)
    {
        read_mtrk(istream,noteCollector);
    }

    return notes;
}
