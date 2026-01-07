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
