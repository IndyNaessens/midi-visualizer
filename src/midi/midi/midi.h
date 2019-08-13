//
// Created by indy on 1/05/19.
//

#ifndef MIDI_PROJECT_MIDI_H
#define MIDI_PROJECT_MIDI_H

#include "primitives.h"
#include <cstdint>
#include <istream>
#include <memory>
#include <functional>
#include <vector>

namespace midi
{
    //CHUNK_HEADER
    struct CHUNK_HEADER
    {
        char id[4];
        uint32_t size;
    };

    void read_chunk_header(std::istream&, CHUNK_HEADER*);
    std::string header_id(const CHUNK_HEADER&);
    //END CHUNK_HEADER

    //MTHD
    #pragma pack(push, 1)
    struct MTHD
    {
        CHUNK_HEADER header;
        uint16_t type;
        uint16_t ntracks;
        uint16_t division;
    };
    #pragma pack(pop)

    void read_mthd(std::istream&, MTHD*);
    //END MTHD

    //MTRK
    bool is_sysex_event(uint8_t event_identifier);
    bool is_meta_event(uint8_t event_identifier);
    bool is_midi_event(uint8_t event_identifier);
    bool is_running_status(uint8_t event_identifier);

    uint8_t extract_midi_event_type(uint8_t midi_event_status);
    Channel extract_midi_event_channel(uint8_t midi_event_status);

    bool is_note_off(uint8_t midi_event_status_type);
    bool is_note_on(uint8_t midi_event_status_type);
    bool is_polyphonic_key_pressure(uint8_t midi_event_status_type);
    bool is_control_change(uint8_t midi_event_status_type);
    bool is_program_change(uint8_t midi_event_status_type);
    bool is_channel_pressure(uint8_t midi_event_status_type);
    bool is_pitch_wheel_change(uint8_t midi_event_status_type);
    bool is_end_of_track_event(uint8_t midi_event_status_type);
    //END MTRK

    //EVENT RECEIVER INTERFACE
    struct EventReceiver
    {
        virtual void note_on(Duration dt, Channel channel, NoteNumber note, uint8_t velocity) = 0;
        virtual void note_off(Duration dt, Channel channel, NoteNumber note, uint8_t velocity) = 0;
        virtual void polyphonic_key_pressure(Duration dt, Channel channel, NoteNumber note, uint8_t pressure) = 0;
        virtual void control_change(Duration dt, Channel channel, uint8_t controller, uint8_t value) = 0;
        virtual void program_change(Duration dt, Channel channel, Instrument program) = 0;
        virtual void channel_pressure(Duration dt, Channel channel, uint8_t pressure) = 0;
        virtual void pitch_wheel_change(Duration dt, Channel channel, uint16_t value) = 0;
        virtual void meta(Duration dt, uint8_t type, std::unique_ptr<uint8_t[]> data, uint64_t data_size) = 0;
        virtual void sysex(Duration dt, std::unique_ptr<uint8_t[]> data, uint64_t data_size) = 0;
    };
    // END EVENT RECEIVER INTERFACE

    void read_mtrk(std::istream&, midi::EventReceiver&);

    //NOTE
    struct NOTE
    {
        NoteNumber note_number;
        Time start;
        Duration duration;
        uint8_t velocity;
        Instrument instrument;

        NOTE(const NoteNumber& noteNumber, const Time& start, const Duration& duration,uint8_t velocity, const Instrument& instrument)
                : note_number(noteNumber),start(start), duration(duration),velocity(velocity), instrument(instrument){};
    };

    bool operator ==(const NOTE&,const NOTE&);
    bool operator !=(const NOTE&,const NOTE&);
    std::ostream& operator << (std::ostream& out, const NOTE&);
    Duration calculate_note_duration(const Time& start_time, const Time& end_time);
    //END NOTE

    //CHANNEL NOTE COLLECTOR
    struct ChannelNoteCollector : EventReceiver
    {
        private:
            Channel current_channel;
            Time current_time;
            Instrument current_instrument;
            std::vector<NOTE> started_notes;
            std::function<void(const NOTE&)> note_receiver;

            void increase_current_time(const Duration& duration);
            void save_note_on(const NOTE& note_on);
            void new_track();

        public:
            ChannelNoteCollector(const Channel& current_channel, std::function<void(const NOTE&)> note_receiver)
                : current_channel(current_channel), note_receiver(std::move(note_receiver)), current_time(0), started_notes(), current_instrument(0){};

            void note_on(Duration dt, Channel channel, NoteNumber note, uint8_t velocity) override;
            void note_off(Duration dt, Channel channel, NoteNumber note, uint8_t velocity) override;
            void polyphonic_key_pressure(Duration dt, Channel channel, NoteNumber note, uint8_t pressure) override;
            void control_change(Duration dt, Channel channel, uint8_t controller, uint8_t value) override;
            void program_change(Duration dt, Channel channel, Instrument program) override;
            void channel_pressure(Duration dt, Channel channel, uint8_t pressure) override;
            void pitch_wheel_change(Duration dt, Channel channel, uint16_t value) override;
            void meta(Duration dt, uint8_t type, std::unique_ptr<uint8_t[]> data, uint64_t data_size) override;
            void sysex(Duration dt, std::unique_ptr<uint8_t[]> data, uint64_t data_size) override;
    };
    //END CHANNEL NOTE COLLECTOR

    //EVENT MULTICASTER
    struct EventMulticaster : EventReceiver
    {
        private:
            std::vector<std::shared_ptr<EventReceiver>> event_receivers;

        public:
            explicit EventMulticaster(std::vector<std::shared_ptr<EventReceiver>> event_receivers) : event_receivers(std::move(event_receivers)){};

            void note_on(Duration dt, Channel channel, NoteNumber note, uint8_t velocity) override;
            void note_off(Duration dt, Channel channel, NoteNumber note, uint8_t velocity) override;
            void polyphonic_key_pressure(Duration dt, Channel channel, NoteNumber note, uint8_t pressure) override;
            void control_change(Duration dt, Channel channel, uint8_t controller, uint8_t value) override;
            void program_change(Duration dt, Channel channel, Instrument program) override;
            void channel_pressure(Duration dt, Channel channel, uint8_t pressure) override;
            void pitch_wheel_change(Duration dt, Channel channel, uint16_t value) override;
            void meta(Duration dt, uint8_t type, std::unique_ptr<uint8_t[]> data, uint64_t data_size) override;
            void sysex(Duration dt, std::unique_ptr<uint8_t[]> data, uint64_t data_size) override;

            void add_event_receiver(const std::shared_ptr<EventReceiver>&);
    };
    //END EVENT MULTICASTER

    //NOTE COLLECTOR
    struct NoteCollector : EventReceiver
    {
        private:
            EventMulticaster event_multicaster;

        public:
            explicit NoteCollector(std::function<void(const NOTE&)>);

            void note_on(Duration dt, Channel channel, NoteNumber note, uint8_t velocity) override;
            void note_off(Duration dt, Channel channel, NoteNumber note, uint8_t velocity) override;
            void polyphonic_key_pressure(Duration dt, Channel channel, NoteNumber note, uint8_t pressure) override;
            void control_change(Duration dt, Channel channel, uint8_t controller, uint8_t value) override;
            void program_change(Duration dt, Channel channel, Instrument program) override;
            void channel_pressure(Duration dt, Channel channel, uint8_t pressure) override;
            void pitch_wheel_change(Duration dt, Channel channel, uint16_t value) override;
            void meta(Duration dt, uint8_t type, std::unique_ptr<uint8_t[]> data, uint64_t data_size) override;
            void sysex(Duration dt, std::unique_ptr<uint8_t[]> data, uint64_t data_size) override;
    };
    //END NOTE COLLECTOR

    std::vector<NOTE> read_notes(std::istream&);
}

#endif //MIDI_PROJECT_MIDI_H
