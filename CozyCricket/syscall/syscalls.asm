BITS 64
default rel

section .data
    SSN         dw 0
    syscallAddr dq 0

section .text
global PrepSyscallNum
PrepSyscallNum:
    mov word [SSN], cx
    ret

global PrepSyscallAddr
PrepSyscallAddr:
    mov qword [syscallAddr], rcx
    ret

global sysNtOpenProcessToken
sysNtOpenProcessToken:
    mov r10, rcx
    mov ax, word [SSN]
    jmp qword [rel syscallAddr]
    ret

global sysNtOpenProcess
sysNtOpenProcess:
    mov r10, rcx
    mov ax, word [SSN]
    jmp qword [rel syscallAddr]
    ret

global sysNtAllocateVirtualMemory
sysNtAllocateVirtualMemory:
    mov r10, rcx
    mov ax, word [SSN]
    jmp qword [rel syscallAddr]
    ret

global sysNtWriteVirtualMemory
sysNtWriteVirtualMemory:
    mov r10, rcx
    mov ax, word [SSN]
    jmp qword [rel syscallAddr]
    ret

global sysmemcpy
sysmemcpy:
    mov r10, rcx
    mov ax, word [SSN]
    jmp qword [rel syscallAddr]
    ret

global sysNtProtectVirtualMemory
sysNtProtectVirtualMemory:
    mov r10, rcx
    mov ax, word [SSN]
    jmp qword [rel syscallAddr]
    ret

global sysNtDelayExecution
sysNtDelayExecution:
    mov r10, rcx
    mov ax, word [SSN]
    jmp qword [rel syscallAddr]
    ret

global sysNtWaitForSingleObject
sysNtWaitForSingleObject:
    mov r10, rcx
    mov ax, word [SSN]
    jmp qword [rel syscallAddr]
    ret

global sysNtQueueApcThread
sysNtQueueApcThread:
    mov r10, rcx
    mov ax, word [SSN]
    jmp qword [rel syscallAddr]
    ret

; INJECT_EXTERNAL_CREATE_MAP_SECTION

global sysNtCreateSection
sysNtCreateSection:
    mov r10, rcx
    mov ax, word [SSN]
    jmp qword [rel syscallAddr]
    ret

global sysNtMapViewOfSection
sysNtMapViewOfSection:
    mov r10, rcx
    mov ax, word [SSN]
    jmp qword [rel syscallAddr]
    ret

global sysNtCreateThreadEx
sysNtCreateThreadEx:
    mov r10, rcx
    mov ax, word [SSN]
    jmp qword [rel syscallAddr]
    ret

; FINDPID_GETNEXTPROCESS
global sysNtGetNextProcess
sysNtGetNextProcess:
    mov r10, rcx
    mov ax, word [SSN]
    jmp qword [rel syscallAddr]
    ret

global sysGetProcessImageFileNameA
sysGetProcessImageFileNameA:
    mov r10, rcx
    mov ax, word [SSN]
    jmp qword [rel syscallAddr]
    ret

global sysPathFindFileNameA
sysPathFindFileNameA:
    mov r10, rcx
    mov ax, word [SSN]
    jmp qword [rel syscallAddr]
    ret
