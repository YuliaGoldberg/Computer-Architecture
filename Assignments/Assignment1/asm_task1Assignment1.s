section	.rodata			; we define (global) read-only variables in .rodata section
	str: db "%s", 10, 0	; format string

section .bss			; we define (global) uninitialized variables in .bss section
	an: resb 12		; enough to store integer in [-2,147,483,648 (-2^31) : 2,147,483,647 (2^31-1)]
	bn: resb 12     ;to help convert the string


section .data
    charCount: dd 0  ;counts how many characters the binary input has

	array:  dd 1
	        dd 2
	        dd 4
	        dd 8
	        dd 16
	        dd 32
	        dd 64
	        dd 128
	        dd 256
	        dd 512
	        dd 1024
	        dd 2048
	        dd 4096
	        dd 8192
	        dd 16384
	        dd 32768
	        dd 65536
	        dd 131072
	        dd 262144
	        dd 524288
	        dd 1048576
	        dd 2097152
	        dd 4194304
	        dd 8388608
	        dd 16777216
	        dd 33554432
	        dd 67108864
	        dd 134217728
	        dd 268435456
	        dd 536870912
	        dd 1073741824
	        dd -2147483648

section .text
	global convertor
	extern printf

convertor:
	push ebp
	mov ebp, esp
	pushad

	mov ecx, dword [ebp+8]	; get function argument (pointer to string)
    mov edx, ecx        ; edx is used to count number of charachters
    mov ebx, 0
    mov esi, 0
    getSize:
        cmp byte [edx],10       ;check if the current char is '\n'
        jz endGetSize          ;if it is, make end of function actions
        inc edx         ; else increment the counter
        jmp getSize     ; repeat the loop
    endGetSize:
    sub edx, ecx ; edx contains the length of the string
    mov dword[charCount], edx  ;counts how many characters the binary input has
    cmp edx, 32   ;check if we got a 32 bit number
    jne endNegetiveHandler  ;if not, continue
    cmp byte[ecx], '1'  ;if it is a 32 bit number, check if negative
    jne endNegetiveHandler  ;if it's not negative, continue to claculate
    mov byte[an], '-'   ;if it is negative, add '-' to the begining of the answer string
    mov esi, 1    ;flag. if it is a negative number, it equals 1
    endNegetiveHandler:
    dec edx ;will now contain the amount of characthers of the input-1
    power:
        cmp byte[ecx], '1'  ; check if the char equals 1
        jne ifEqualsZero  ;if it is not 1, add nothing to the sum
        add ebx,[array+4*edx]  ;if it is 1, add
        ifEqualsZero:
        dec edx    ;decrease the power
        inc ecx  ; read the next char
        cmp edx, -1   ;check if it is the end of the loop, if not do the loop again
        jne power    ;do the loop again
    mov eax, ebx   ;eax will now contain the value of the number
    mov ecx, 10 ;ecx will contain the number we want to devide by
    mov ebx, 0   ;the place in the string
    cmp esi,1   ;check if the input number is negative
    jne divide ;if not, continue
    neg eax     ;if negative -find 2's complement
    divide:
        mov edx, 0  ;initialize the register. it will contain the remainder
        cmp dword[charCount], 0  ;check if we read the whole input
        jz endDivide
        div ecx    ;divide the number
        add dl, 48   ;convert to char
        mov [bn+ebx], dl   ;insert to 'help' array
        inc ebx   ;move to the next cell
        dec dword[charCount] ;decrease the amount of the charachters need to be read from the string
        cmp al,0  ;check if we finished dividing the number
        jz endDivide
        jmp divide
    endDivide:
            dec ebx  ;decrease by one so we could read the value and put it in 'an'
            mov edx, esi   ;if positive number- edx=0, else edx=1
            mov ecx, 0    ;initialize register. will contain the value in a certain cell
     swap:
        cmp ebx, -1  ;check if we finished tranfering the string fron 'bn' to 'an'
        je finish
        mov cl, [bn+ebx]   ;transfer the value of the cell at bn from bn using cl
        mov [an+edx], cl   ;transfer fron cl to the cell in an
        mov ecx, 0   ;initialize register. will contain the value in a certain cell
        dec ebx   ;go to the next cell the needs to be copied
        inc edx   ;go to the next cell the needs to be copied to
        jmp swap

    finish:
    mov byte[an+edx], 0  ;insert so printf will know the string has ended
	push an			; call printf with 2 arguments -
	push str			; pointer to str and pointer to format string
	call printf
	add esp, 8		; clean up stack after call

	popad
	mov esp, ebp
	pop ebp
	ret