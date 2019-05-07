section	.rodata			; we define (global) read-only variables in .rodata section
	str: db "%s", 10, 0	; format string
	int: db "%d", 10, 0	; format string

section .data
    illegal:db 'i'
            db 'l'
            db 'l'
            db 'e'
            db 'g'
            db 'a'
            db 'l'
            db ' '
            db 'i'
            db 'n'
            db 'p'
            db 'u'
            db 't'
            db 0

section .text
	global assFunc
	extern printf
	extern c_checkValidity

assFunc:
    push ebp ; backup EBP
    mov ebp, esp ; reset EBP to the current ESP
    mov ecx, [ebp+8] ; get first argument
    mov ebx, [ebp+12] ; get second argument
    push ebx ; push second argument
    push ecx ; push first argument
    call c_checkValidity ; call the function → push return address
    add esp, 8		; clean up stack after call
    cmp eax,0  ;will be 0 if the input is illegal. 1 otherwise
    je printIllegal
    add ecx, ebx  ;calculate the sum of the two inserted numbers
    push ecx   ;push first arguement of printf
	push int		;push second arguement of printf
	call printf
	add esp, 8		; clean up stack after call
    mov esp, ebp
    pop ebp
    ret



printIllegal:
    push illegal   ;push first arguement of printf
    push str
    call printf
    add esp,8
    mov esp, ebp
    pop ebp
    ret




