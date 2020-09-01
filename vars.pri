DEFINES += RAT_TEST_PAD

CONFIG(release, debug|release): BUILD=release
CONFIG(debug, debug|release): BUILD=debug

windows:{
    PLATFORM_OS="windows"
    PLATFORM_OS_U="WINDOWS"
    PLATFORM_OS_W="Windows"
    contains(QMAKE_TARGET.arch, x86_64):{
        PLATFORM_ARCH = "64"
    }else{
        PLATFORM_ARCH = "32"
    }
}else{
    linux: {
        PLATFORM_OS = "linux"
        PLATFORM_OS_U="LINUX"
        PLATFORM_OS_W="Linux"
    }
    macx: {
        PLATFORM_OS = "mac"
        PLATFORM_OS_U="MAC"
        PLATFORM_OS_W="Mac"
    }

    BITSIZE = $$system(getconf LONG_BIT)
    if (contains(BITSIZE, 64)) {
        PLATFORM_ARCH = "64"
    }
    if (contains(BITSIZE, 32)) {
        PLATFORM_ARCH = "32"
    }
    android: {
        PLATFORM_OS = "android"
        PLATFORM = "android"
        PLATFORM_OS_U="ANDROID"
        PLATFORM_OS_W="Android"
    }
}

DEFINES += "RAT_OS_$${PLATFORM}"
export(PLATFORM)
export(PLATFORM_OS)
export(PLATFORM_ARCH)

isEmpty(PLATFORM) {
    PLATFORM = "$$PLATFORM_OS-$$PLATFORM_ARCH"
}

message(======================)
message(Generating for $$PLATFORM)


