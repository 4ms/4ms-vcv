add_library(flatbuffers STATIC)

add_library(flatbuffer_messages INTERFACE)
target_include_directories(flatbuffer_messages INTERFACE ${CMAKE_CURRENT_LIST_DIR}/flat)

target_sources(flatbuffers PRIVATE encode.cc)

target_include_directories(flatbuffers PUBLIC 
    ./
    ${CMAKE_CURRENT_LIST_DIR}/../../lib/flatbuffers/include
)


