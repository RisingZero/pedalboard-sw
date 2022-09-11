add_library(RtMidiLib RtMidi.cpp RtMidi.h)
if (APPLE)
    add_compile_definitions(__MACOSX_CORE__)
    target_link_libraries(RtMidiLib
        "-framework CoreMidi"
        "-framework CoreAudio"
        "-framework CoreFoundation"
    )
elseif(UNIX and not APPLE)
    add_compile_definitions(__LINUX_ALSA__)
    target_link_libraries(RtMidiLib
        "-lasound"
        "-lpthread"
    )
endif()