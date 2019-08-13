cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 17)

project(midi-project)

set(dir src/midi)
set(testdir src/midi/tests)

set(TEST
        ${testdir}/tests.cpp
        ${testdir}/01-io/01-endianness-tests.cpp
        ${testdir}/01-io/02-read-to-tests.cpp
        ${testdir}/01-io/03-read-tests.cpp
        ${testdir}/01-io/04-read-array-tests.cpp
        ${testdir}/01-io/05-read-variable-length-integer-tests.cpp
        ${testdir}/02-midi/01-primitives/01-channel-tests.cpp
        ${testdir}/02-midi/01-primitives/02-channel-show-tests.cpp
        ${testdir}/02-midi/01-primitives/03-instruments-tests.cpp
        ${testdir}/02-midi/01-primitives/04-note-number-tests.cpp
        ${testdir}/02-midi/01-primitives/05-time-tests.cpp
        ${testdir}/02-midi/02-chunk-headers/01-chunk-header-tests.cpp
        ${testdir}/02-midi/02-chunk-headers/02-read-chunk-header-tests.cpp
        ${testdir}/02-midi/02-chunk-headers/03-header-id-tests.cpp
        ${testdir}/02-midi/03-mthd/01-mthd-tests.cpp
        ${testdir}/02-midi/03-mthd/02-read-mthd-tests.cpp
        ${testdir}/02-midi/04-mtrk/01-mtrk-auxiliary-tests.cpp
        ${testdir}/02-midi/04-mtrk/02-mtrk-event-receiver-tests.cpp
        ${testdir}/02-midi/04-mtrk/03-mtrk-empty-tests.cpp
        ${testdir}/02-midi/04-mtrk/04-mtrk-meta-tests.cpp
        ${testdir}/02-midi/04-mtrk/05-mtrk-sysex-tests.cpp
        ${testdir}/02-midi/04-mtrk/06-mtrk-note-off-tests.cpp
        ${testdir}/02-midi/04-mtrk/07-mtrk-note-on-tests.cpp
        ${testdir}/02-midi/04-mtrk/08-mtrk-polyphonic-tests.cpp
        ${testdir}/02-midi/04-mtrk/09-mtrk-control-change-tests.cpp
        ${testdir}/02-midi/04-mtrk/10-mtrk-program-change-tests.cpp
        ${testdir}/02-midi/04-mtrk/11-mtrk-channel-pressure-tests.cpp
        ${testdir}/02-midi/04-mtrk/12-mtrk-pitch-wheel-tests.cpp
        ${testdir}/02-midi/04-mtrk/13-mtrk-multiple-events-tests.cpp
        ${testdir}/02-midi/05-notes/01-note-tests.cpp
        ${testdir}/02-midi/05-notes/02-channel-note-collector-tests.cpp
        ${testdir}/02-midi/05-notes/03-event-multicaster-tests.cpp
        ${testdir}/02-midi/05-notes/04-note-collector-tests.cpp
        ${testdir}/02-midi/05-notes/05-read-notes-tests.cpp)

set(STUDENT-TEST
        ${dir}/io/endianness.cpp
        ${dir}/io/vli.cpp
        ${dir}/midi/primitives.cpp
        ${dir}/midi/midi.cpp)

set(APP
        ${dir}/app.cpp)

set(LOG
        ${dir}/easylogging++.cpp
        ${dir}/logging.cpp)

set(IMAGING
        ${dir}/imaging/bitmap.cpp
        ${dir}/imaging/bmp-format.cpp
        ${dir}/imaging/color.cpp)

set(SHELL
        ${dir}/shell/command-line-parser.cpp)

set(RENDERING
        ${dir}/rendering/renderer.cpp)

#test
add_executable(midi-student-test)
target_compile_definitions(midi-student-test PRIVATE TEST_BUILD)
target_sources(midi-student-test PRIVATE ${STUDENT-TEST} ${TEST} ${LOG})
target_include_directories(midi-student-test PRIVATE ${dir})

#app
add_executable(midi-student)
target_sources(midi-student PRIVATE ${APP} ${RENDERING} ${STUDENT-TEST} ${IMAGING} ${SHELL} ${LOG})
target_include_directories(midi-student PRIVATE ${dir})