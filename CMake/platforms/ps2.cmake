set(NONET ON)
set(DISABLE_DEMOMODE ON)
set(ASAN OFF)
set(UBSAN OFF)
set(BUILD_TESTING OFF)

set(DEVILUTIONX_SYSTEM_SDL2 OFF)
set(DEVILUTIONX_SYSTEM_BZIP2 OFF)
set(DEVILUTIONX_SYSTEM_LIBFMT OFF)
set(PREFILL_PLAYER_NAME ON)
set(NOEXIT ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/threads-stub")

set(BUILD_ASSETS_MPQ OFF)
set(UNPACKED_MPQS ON)

# -fmerge-all-constants saves ~4 KiB
set(_extra_flags "-fmerge-all-constants -fipa-pta")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${_extra_flags}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${_extra_flags}")

