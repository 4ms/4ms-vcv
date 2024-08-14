add_library(
  jansson STATIC
  jansson/src/dump.c
  jansson/src/error.c
  jansson/src/hashtable.c
  jansson/src/hashtable_seed.c
  jansson/src/load.c
  jansson/src/memory.c
  jansson/src/pack_unpack.c
  jansson/src/strbuffer.c
  jansson/src/strconv.c
  jansson/src/utf.c
  jansson/src/value.c
  jansson/src/version.c
)

target_include_directories(jansson PUBLIC jansson/src ./)
target_compile_definitions(jansson PRIVATE -DHAVE_CONFIG_H)
