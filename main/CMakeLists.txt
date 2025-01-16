
set(binary_files
    "../binaries/bootloader.bin"
    "../binaries/partition-table.bin"
    "../binaries/app.bin"
)

idf_component_register(
    SRCS main.cpp
    EMBED_FILES ${binary_files}
)