idf_component_register(
    SRCS src/flasher.cpp
    INCLUDE_DIRS "include"
    REQUIRES esp-serial-flasher driver
)

