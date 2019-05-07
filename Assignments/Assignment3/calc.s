  section	.rodata			; we define (global) read-only variables in .rodata section
  	str: db "%s", 10, 0	; format string
  	debugPrint: db "debugMode: ",0
    int: db "%X",0	; format int
    num: db "%X",10,0	; format int
    empty:  db ' ',0
    errorEmpty: db 'Error: Insufficient Number of Arguments on Stack',0
    errorFull: db 'Error: Operand Stack Overflow',0
    errorGreater: db 'Error: 2nd operand is greater than 200',0


  section .data
    stackSize: equ 5
    counter: dd 0
    temp: dd 0
    hexHolder: db 0
    buffIndex: dd 0
    stackIndex: dd 0
    currLink: dd 0
    headLink: dd 0
    stringSize: dd 0
    isEven: dw 0
    countPush: dd 0
    isFirst: dd 1
    list1Size: dd 0
    list2Size: dd 0
    currList1Size: dd 0
    currList2Size: dd 0
    carry: dd 0
    addressFound: dd 0
    saveAddress: dd 0
    next: dd 0
    firstNonZero: dd 0
    countActions: dd 0
    isDebugMode: dd 0


 section .bss
    buffer: resb 80
    stack: resd stackSize

  section .text
     align 16
     global main
     extern printf
     extern fflush
     extern malloc
     extern calloc
     extern free
     extern gets
     extern fgets
     extern stderr
     extern fprintf



;******************************************** functions ****************************************************************

    %macro handleCarry 0
            mov byte[carry], 0
            jc %%changeCarry
            jmp %%finishMac
            %%changeCarry:
            mov byte[carry], 1
            %%finishMac:
        %endmacro

        %macro popAndCreate 0
            ;countPush holds the amount of objects in stack
            %%create:
            pop ecx ;ecx now contains the number
            pushad
            pushfd
            push 1
            push 5
            call calloc     ;eax will now point on 40 bits initialized
            add esp, 8
            mov dword[saveAddress], eax
            popfd
            popad
            mov eax, dword[saveAddress]
            mov byte[eax], cl   ;  the begining of eax is the number
            mov ebx, dword[currLink] ;tranfer the address
            mov dword[eax+1], ebx ;the rest of eax is the "next address"
            mov dword[currLink], eax ; currLink will point on the first link
            dec dword[countPush]
            cmp dword[countPush], 0
            jne %%create
            mov ebx, dword[stackIndex]
            mov dword[stack+ebx],eax
            add ebx,4
            mov dword[stackIndex], ebx
            deBugModeList
        %endmacro

    %macro getListSize 0
         mov edx,0
         %%loop4:
         cmp ecx,0 ;ecx is the address of the link
         je %%retur
         mov ecx,dword[ecx+1]
         inc edx
         jmp %%loop4
         %%retur:
         %endmacro

    %macro deBugModeList 0
    pushad
    cmp dword[isDebugMode],1
    jne %%endDeBugModeList
    push debugPrint
    push dword[stderr]
    call fprintf
    add esp,8
    mov esi,dword[stackIndex]
    sub esi,4
    mov eax, dword[stack+esi] ;eax is the address of the number in the top of the stack
    mov ecx,eax
    mov dword[firstNonZero],ecx
        %%cleanZeroes1:
            cmp ecx,0
            je %%freeZeroes1
            mov bl,byte[ecx]
            cmp bl,0
            je %%zero1
            mov dword[firstNonZero],ecx
            %%zero1:
            mov ecx,dword[ecx+1]
            jmp %%cleanZeroes1
        %%freeZeroes1:
            mov ecx,dword[firstNonZero]
            mov ecx,dword[ecx+1]
            %%freeZeroesLoop1:
            cmp ecx,0
            je %%endfreeZeroesLoop1
            mov edx,dword[ecx+1]
            pushad
            push ecx
            call free
            add esp,4
            popad
            mov ecx,edx
            jmp %%freeZeroesLoop1
            %%endfreeZeroesLoop1:
            mov ecx,dword[firstNonZero]
            mov dword[ecx+1],0
        %%loop21:
        mov edx,0
        mov dl,byte[eax] ;insert the value in the stack
        push edx
        inc dword[countPush] ;count how many pushes
        cmp edx,16
        jge %%printOneDigit1
        cmp dword[eax+1],0
        je %%printOneDigit1
        push 0
        inc dword[countPush]
        %%printOneDigit1:
        mov eax,dword[eax+1]
        cmp eax, 0 ;is it the last link?
        jne %%loop21 ; if not, do the loop again

        %%printLoop1:
        push int
        push dword[stderr]
        call fprintf
        add esp,12
        dec dword[countPush]
        cmp dword[countPush],0
        jne %%printLoop1
        push empty
        push str
        push dword[stderr]
        call fprintf
        add esp,12
        %%endDeBugModeList:
        popad
     %endmacro

    %macro freeRedundantZeros 0
    pushad
        mov ecx,eax
        mov dword[firstNonZero],ecx
            %%cleanZeroes1:
                cmp ecx,0
                je %%freeZeroes1
                mov bl,byte[ecx]
                cmp bl,0
                je %%zero1
                mov dword[firstNonZero],ecx
                %%zero1:
                mov ecx,dword[ecx+1]
                jmp %%cleanZeroes1
            %%freeZeroes1:
                mov ecx,dword[firstNonZero]
                mov ecx,dword[ecx+1]
                %%freeZeroesLoop1:
                cmp ecx,0
                je %%endfreeZeroesLoop1
                mov edx,dword[ecx+1]
                pushad
                push ecx
                call free
                add esp,4
                popad
                mov ecx,edx
                jmp %%freeZeroesLoop1
                %%endfreeZeroesLoop1:
                mov ecx,dword[firstNonZero]
                mov dword[ecx+1],0
    popad
    %endmacro

    stackEmptyMsg:
    push errorEmpty
    push str
    call printf
    add esp, 8
    jmp mainLoop

    stackOverFlowMsg:
    push errorFull
    push str
    call printf
    add esp, 8
    jmp mainLoop

    greaterThanError:
    push errorGreater
    push str
    call printf
    add esp, 8
    jmp mainLoop


convert:
    mov eax,0
    mov ebx, 0
    mov edx, dword[buffIndex]
    loop:
    cmp word[isEven],0
    je caseEven
    inc ebx
    caseEven:
    mov ecx, 0
    mov cl, [buffer+edx]
    cmp cl, 57 ;;check if a number or a letter
    jnbe caseLetter

    caseNum:
    sub cl, 48
    jmp handle

    caseLetter:
    cmp cl, 'A'
    jne caseB
    mov cl, 10
    jmp handle
    caseB:
    cmp cl, 'B'
    jne caseC
    mov cl, 11
    jmp handle
    caseC:
    cmp cl, 'C'
    jne caseD
    mov cl, 12
    jmp handle
    caseD:
    cmp cl, 'D'
    jne caseE
    mov cl, 13
    jmp handle
    caseE:
    cmp cl, 'E'
    jne caseF
    mov cl, 14
    jmp handle
    caseF:
    cmp cl, 'F'
    jne handle
    mov cl, 15

    handle:
    cmp ebx,0
    jne second
    shl ecx,4
    second:
    add eax,ecx
    mov word[isEven], 0

    finish:
    inc dword[buffIndex]
    inc edx
    inc ebx
    cmp ebx,2
    je endloop
    jmp loop
    endloop:
    mov byte[hexHolder],al
    jmp return

;******************************************* myCalc() ******************************************************************

myCalc:
push    ebp             ; Save caller state
mov     ebp, esp
sub esp,4
mov eax,0
initializeStack:
    mov dword[stack+eax*4], 0
    inc eax
    cmp eax, 5
    jnz initializeStack
mainLoop:
    mov dword[buffIndex], 0
    mov eax,0
    initializeBuffer:
          mov byte[buffer+eax], 0
          inc eax
          cmp eax, 80
          jnz initializeBuffer
    getString:
        push buffer
        call gets
        add esp,4
        mov eax,0
    getSize:
        loop1:
            cmp byte[buffer+eax],0       ;check if the current char is '\n'
            jz endLoop1          ;if it is, make end of function actions
            inc eax         ; else increment the counter
            jmp loop1     ; repeat the loop
            endLoop1:
        mov dword[stringSize],eax
    checkIfEven:
        mov edx,0
        mov ecx,2
        div cx
        mov word[isEven],dx

;***************************************************** quit ************************************************************
    caseQuit:
        cmp byte[buffer],'q'
        jne casePlus
        sub dword[stackIndex],4
        stackLoop:
        cmp dword[stackIndex],-4
        je endStackLoop
        mov esi, dword[stackIndex]
        mov eax,dword[stack+esi] ;the address
        freeMe:
             mov ebx,dword[eax+1]
             pushad
             push eax
             call free
             add esp,4
             popad
             mov eax,ebx
             cmp eax,0
             je endFreeMeLoop
             jmp freeMe
        endFreeMeLoop:
        mov dword[stack+esi],0
        sub dword[stackIndex],4
        jmp stackLoop
        endStackLoop:
        jmp endProgram

;***************************************************** addition ********************************************************
    casePlus:
        mov byte[currLink],0
        cmp byte[buffer],'+'
        jne casePopAndPrint
        inc dword[countActions]
        cmp dword[stackIndex],8
        jb stackEmptyMsg
        mov dword[countPush], 0
        mov esi,dword[stackIndex]
        sub esi,4
        mov eax,dword[stack+esi] ;eax points on the first number
        sub esi,4
        mov ebx,dword[stack+esi] ;ebx points on the second number
        mov ecx, eax ;  ecx will now point on the first number
        getListSize
        mov dword[list1Size], edx
        mov ecx, ebx ;  ecx will now point on the second number
        getListSize
        mov dword[list2Size], edx
        loop3:
        mov ecx, 0
        mov edx,0
        mov ecx, dword[list1Size]
        add ecx, dword[list2Size]
        cmp ecx, 0
        je newLink
        mov ecx,0
        mov edx,0
        cmp dword[list1Size], 0
        je list1Empty
        mov cl, byte[eax]
        mov esi, eax
        mov eax, dword[eax+1]
        pushad
        push esi
        call free
        add esp,4
        popad
        list1Empty:
        cmp dword[list2Size], 0
        je list2Empty
        mov dl, byte[ebx]
        mov esi, ebx
        mov ebx, dword[ebx+1]
        pushad
        push esi
        call free
        add esp,4
        popad
        list2Empty:
        add cl, byte[carry]
        handleCarry
        cmp dword[carry], 0
        je carryzero
        add cl, dl
        jmp continue

        carryzero:
        add cl,dl
        handleCarry
        continue:

        mov edx,0
        mov dl,cl
        push edx
        inc dword[countPush]
        cmp dword[list1Size],0
        je empty1
        dec dword[list1Size]
        empty1:
        cmp dword[list2Size],0
        je empty2
        dec dword[list2Size]
        empty2:
        jmp loop3

    newLink:
        cmp dword[carry], 1
        jne create
        mov edx, dword[carry]
        push edx
        inc dword[countPush]
       create:
        mov esi,dword[stackIndex]
        sub esi,4
        mov dword[stack+esi],0 ;remove the list from stack
        sub esi,4
        mov dword[stack+esi],0 ;remove the list from stack
        sub dword[stackIndex], 8
        popAndCreate

        mov dword[countPush], 0
        mov dword[currLink],0
        mov dword[carry], 0
        mov dword[list1Size], 0
        mov dword[list2Size], 0
        jmp mainLoop

;***************************************************** pop *************************************************************
    casePopAndPrint:
    cmp byte[buffer],'p'
    jne caseMul
    pop:
    sub dword[stackIndex],4
    mov esi,dword[stackIndex]
    cmp dword[stackIndex], -4 ;is the stack empty?
    jg notEmpty
    add dword[stackIndex],4
    je stackEmptyMsg ;if true, throw a message
    notEmpty:
    inc dword[countActions]
    mov eax, dword[stack+esi] ;eax is the address of the number in the top of the stack
    mov ecx,eax
    mov dword[firstNonZero],ecx
        cleanZeroes:
            cmp ecx,0
            je freeZeroes
            mov bl,byte[ecx]
            cmp bl,0
            je zero
            mov dword[firstNonZero],ecx
            zero:
            mov ecx,dword[ecx+1]
            jmp cleanZeroes
        freeZeroes:
            mov ecx,dword[firstNonZero]
            mov ecx,dword[ecx+1]
            freeZeroesLoop:
            cmp ecx,0
            je endfreeZeroesLoop
            mov edx,dword[ecx+1]
            pushad
            push ecx
            call free
            add esp,4
            popad
            mov ecx,edx
            jmp freeZeroesLoop
            endfreeZeroesLoop:
            mov ecx,dword[firstNonZero]
            mov dword[ecx+1],0
        loop2:
        mov edx,0
        mov dl,byte[eax] ;insert the value in the stack
        push edx
        inc dword[countPush] ;count how many pushes
        cmp edx,16
        jge printOneDigit
        cmp dword[eax+1],0
        je printOneDigit
        push 0
        inc dword[countPush]
        printOneDigit:
        mov edx, eax
        mov eax, dword[eax+1] ;take the address of the new link
        pushad
        push edx
        call free
        add esp,4
        popad
        cmp eax, 0 ;is it the last link?
        jne loop2 ; if not, do the loop again
        cmp byte[buffer],'p'
        je printLoop
        mov dword[stack+esi],0
        jmp mainLoop
        printLoop:
        push int
        call printf
        add esp,8
        dec dword[countPush]
        cmp dword[countPush],0
        jne printLoop
        push empty
        push str
        call printf
        add esp,8
        mov dword[stack+esi],0
        jmp mainLoop


;**************************************************** multiplication ***************************************************
    caseMul:
    cmp byte[buffer],'^'
    jne caseDiv
    inc dword[countActions]
    cmp dword[stackIndex],8
    jl stackEmptyMsg
    check2ndArg:
        mov esi,dword[stackIndex]
        sub esi,8
        mov eax,dword[stack+esi] ;eax contains pointer to the number of shifts to do
        freeRedundantZeros
        mov ebx,0
        mov bl,byte[eax] ;ebx contains the number of shifts to do
        cmp ebx,200
        jge greaterThanError
        mov ecx,dword[eax+1]
        cmp ecx,0
        jne greaterThanError
    endCheck2ndArg:
    add esi,4
    mov eax,dword[stack+esi] ;eax contains pointer to the number to shift
    mov dword[carry],0 ;carry contains the carry from the previeus link
    mulLoop:
        cmp ebx,0
        je endMulLoop
        shlLoop:
            cmp eax,0
            je endShlLoop
            mov dl,byte[eax]
            mov cl,dl
            shl dl,1
            or dl,byte[carry]
            mov byte[eax],dl ;assign the shift number to the curr link
            mov dword[temp],eax ;save the previeus link address in temp
            mov eax,dword[eax+1]
            mov dword[carry],0 ;zeroing the carry flag
            and cl,128 ;check if there is a carry
            cmp cl,128 ;if so, assign 1 to carry variable
            jne shlLoop
            mov dword[carry],1
            jmp shlLoop
        endShlLoop:
        addLink:
        pushad
        cmp dword[carry],0
        je endAddLink
        push 5
        push 1
        call calloc
        add esp,8
        mov edx,dword[temp]
        mov dword[edx+1],eax
        mov byte[eax],1
        endAddLink:
        popad
        dec ebx
        mov eax,dword[stack+esi]
        mov dword[carry],0
        jmp mulLoop
    endMulLoop:
    clearMemory:
    sub esi,4
    mov eax,dword[stack+esi]
    pushad
    push eax
    call free
    add esp,4
    popad
    mov ebx,esi
    add ebx,4
    add ebx,stack
    mov ecx,dword[ebx]
    mov dword[ebx],0
    mov dword[stack+esi],ecx
    sub dword[stackIndex],4
    deBugModeList
    jmp mainLoop

;************************************************** division ***********************************************************
    caseDiv:
    cmp byte[buffer],'v'
    jne caseN
    inc dword[countActions]
    cmp dword[stackIndex],8
    jl stackEmptyMsg
    checkArg:
        mov esi,dword[stackIndex]
        sub esi,8
        mov eax,dword[stack+esi] ;eax contains pointer to the number of shifts to do
        freeRedundantZeros
        mov ebx,0
        mov bl,byte[eax] ;ebx is the number of shifts to do
        cmp ebx,200
        jge greaterThanError
        mov ecx,dword[eax+1]
        cmp ecx,0
        jne greaterThanError
    endCheckArg:
    add esi,4
    mov eax,dword[stack+esi] ;eax contains pointer to the number to shift
    mov edi,dword[eax+1] ;edi contains the next link address
    divLoop:
    cmp ebx,0
    je endDivLoop
        shrLoop:
            cmp eax,0
            je endShrLoop
            mov cl,byte[eax]
            shr cl,1
            cmp edi,0
            je lastLink
            mov dl,byte[edi]
            and dl,1
            shl dl,7
            or cl,dl
            mov byte[eax],cl
            mov eax,edi
            mov edi,dword[eax+1]
            jmp shrLoop
            lastLink:
            mov byte[eax],cl
            cmp cl,0
            jne endShrLoop
            mov dword[eax+1],0
        endShrLoop:
    dec ebx
    mov eax,dword[stack+esi] ;eax contains pointer to the number to shift
    mov edi,dword[eax+1] ;edi contains the next link address
    jmp divLoop
    endDivLoop:
    clearMem:
    sub esi,4
    mov eax,dword[stack+esi]
    pushad
    push eax
    call free
    add esp,4
    popad
    mov ebx,esi
    add ebx,4
    add ebx,stack
    mov ecx,dword[ebx]
    mov dword[ebx],0
    mov dword[stack+esi],ecx
    sub dword[stackIndex],4
    deBugModeList
    jmp mainLoop

;************************************************ 1BitCounter **********************************************************
    caseN:
    cmp byte[buffer],'n'
    jne caseSquare
    mov esi,dword[stackIndex]
    cmp esi,0
    je stackEmptyMsg
    inc dword[countActions]
    mov dword[counter],0
    mov esi,dword[stackIndex]
    sub esi,4
    mov eax,dword[stack+esi]
    loopN:
        cmp eax,0
        je insert
        mov dl,byte[eax]
        mov ebx,0
        shiftLoop:
            mov ecx,0
            mov cl,1
            cmp ebx,8
            je endShiftLoop
            and cl,dl
            cmp cl,1
            jne zeroBit
            inc dword[counter]
            mov esi, dword[counter]
            zeroBit:
            shr dl,1
            inc ebx
            jmp shiftLoop
        endShiftLoop:
        mov eax, dword[eax+1]
        jmp loopN
    insert:
        mov dword[countPush],0
        mov eax,dword[counter]
        cmp eax,0
        jne insertLoop
        push 0
        inc dword[countPush]
        jmp endInsertLoop
        insertLoop:
        cmp eax,0
        je endInsertLoop
        mov ebx,0
        or bl,al
        push ebx
        inc dword[countPush]
        shr eax,8
        jmp insertLoop
    endInsertLoop:
    freeMemory:
        mov edx,dword[stackIndex]
        sub edx, 4
        mov eax,dword[stack+edx]
        freeLoop:
            mov ebx,dword[eax+1]
            pushad
            push eax
            call free
            add esp,4
            popad
            mov eax,ebx
            cmp eax,0
            je endFreeLoop
            jmp freeLoop
        endFreeLoop:
        mov dword[stack+edx],0
        mov dword[stackIndex],edx
    popLoop:
        cmp dword[countPush],0
        je endCaseN
        push 1
        push 5
        call calloc
        add esp,8
        mov esi,dword[stackIndex]
        mov ebx,dword[stack+esi]
        mov dword[temp],ebx
        mov dword[stack+esi],eax
        pop edx
        mov byte[eax],dl
        mov ebx,dword[temp]
        mov dword[eax+1],ebx
        dec dword[countPush]
        jmp popLoop
    endCaseN:
    add dword[stackIndex],4
    deBugModeList
    jmp mainLoop

;***********************************************************************************************************************
    caseSquare:
    cmp byte[buffer],'s'
    jne caseDuplicate
    inc dword[countActions]
    jmp mainLoop

;*********************************************** duplicate *************************************************************
    caseDuplicate:
    cmp byte[buffer],'d'
    jne caseNumber
    mov eax,4
    mov edx,stackSize
    mul edx
    cmp dword[stackIndex],eax
    je stackOverFlowMsg
    mov eax, 0
    mov edx, 0
    mov esi,dword[stackIndex]
    cmp esi,0
    je stackEmptyMsg
    inc dword[countActions]
    sub esi,4
    mov ecx,dword[stack+esi]
    pushad
    push 5
    push 1
    call calloc
    add esp,8
    mov esi,dword[stackIndex]
    mov dword[stack+esi],eax
    popad
    mov esi,dword[stackIndex]
    mov edx,dword[stack+esi]
    loopDup:
        mov bl,byte[ecx]
        mov byte[edx],bl
        cmp dword[ecx+1],0
        je endLoopDup
        pushad
        push 5
        push 1
        call calloc
        add esp,8
        mov dword[temp],eax
        popad
        mov ebx,dword[temp]
        mov dword[edx+1],ebx
        mov ecx,dword[ecx+1]
        mov edx,dword[edx+1]
        jmp loopDup
        endLoopDup:
        deBugModeList
    add dword[stackIndex],4
    jmp mainLoop

;********************************************** insert number **********************************************************
    caseNumber:
    mov eax,4
    mov edx,stackSize
    mul edx
    cmp dword[stackIndex],eax
    je stackOverFlowMsg
    mov eax, 0
    mov edx, 0
    jmp convert
    return:
    push 1
    push 5
    call calloc     ;eax will now point on 40 bits initialized
    add esp, 8

    mov bl, byte[hexHolder] ;transfer the number to bl
    mov byte[eax], bl   ;  the begining of eax is the number
    mov ebx, dword[currLink] ;tranfer the address
    mov dword[eax+1], ebx ;the rest of eax is the "next address"
    mov dword[currLink], eax ; currLink will point on the first link
    mov ecx, dword[buffIndex]
    cmp ecx, dword[stringSize] ;end of buffer?
    jne caseNumber
    mov ecx, dword[currLink]
    mov edx, dword[stackIndex]
    mov dword[stack+edx], ecx
    add edx, 4
    mov dword[stackIndex], edx
    deBugModeList
    mov dword[currLink],0
    mov dword[countPush],0
    mov byte[hexHolder],0
    jmp mainLoop

    endProgram:
    mov ebx,dword[countActions]
    mov [ebp-4],ebx
    mov eax,[ebp-4]
    mov esp, ebp
	pop ebp

	ret



;*************************************************** main **************************************************************

main:
    push ebp
    mov ebp,esp
    mov eax,dword[esp+8]
    cmp eax,1
    je notDebugMode
    mov eax,dword[esp+12]
    mov ebx,[eax+4]
    cmp word[ebx],"-d"
    jne notDebugMode
    mov dword[isDebugMode],1
    notDebugMode:
    call myCalc
    push eax
    push num
    call printf
    add esp,8
    mov esp,ebp
    pop ebp
    ret






