.686P
.MODEL flat, stdcall
.STACK 4096
option casemap:none

printf          PROTO C :dword,:vararg
ExitProcess	PROTO :DWORD

INCLUDELIB C:\Irvine\USER32.LIB
INCLUDELIB C:\Irvine\KERNEL32.LIB
INCLUDELIB C:\Irvine\MSVCRT.LIB

exit equ <INVOKE ExitProcess,0>

chr$ MACRO any_text:vararg
	LOCAL textname
.data
	textname db any_text,0
	ALIGN 4
.code
	EXITM<OFFSET textname>
ENDM

.data
score dword 24 dup(?)
credit dword 24 dup(?)
mean dword ?
sum dword ?
temp dword ?
i dword ?

.code
main PROC
L0:	mov [score], 76
L1:	mov [score+4], 82
L2:	mov [score+8], 90
L3:	mov [score+12], 86
L4:	mov [score+16], 79
L5:	mov [score+20], 62
L6:	mov [credit], 2
L7:	mov [credit+4], 2
L8:	mov [credit+8], 1
L9:	mov [credit+12], 2
L10:	mov [credit+16], 2
L11:	mov [credit+20], 3
L12:	mov [temp], 0
L13:	mov [sum], 0
L14:	mov [i], 0
L15:	mov eax, [i]
L16:	cmp eax, 6
L17:	jb L20
L18:	mov eax, 0
L19:	jmp L21
L20:	mov eax, 1
L21:	cmp eax, 0
L22:	jnz L26
L23:	jz L37
L24:	inc [i]
L25:	jmp L15
L26:	mov esi, [i]
L27:	mov eax, [score+esi*4]
L28:	push eax
L29:	mov esi, [i]
L30:	mov eax, [credit+esi*4]
L31:	mov ebx, eax
L32:	pop eax
L33:	mul ebx
L34:	add eax, [sum]
L35:	mov [sum], eax
L36:	jmp L24
L37:	mov eax, [i]
L38:	cmp eax, 0
L39:	jnz L42
L40:	mov eax, 0
L41:	jmp L43
L42:	mov eax, 1
L43:	cmp eax, 0
L44:	jz L53
L45:	mov eax, [i]
L46:	sub eax, 1
L47:	mov esi, eax
L48:	mov eax, [credit+esi*4]
L49:	add eax, [temp]
L50:	mov [temp], eax
L51:	dec [i]
L52:	jmp L37
L53:	mov edx, 0
L54:	mov eax, [sum]
L55:	mov ebx, [temp]
L56:	div ebx
L57:	mov [mean], eax
L58:	mov eax, [mean]
L59:	cmp eax, 60
L60:	jae L63
L61:	mov eax, 0
L62:	jmp L64
L63:	mov eax, 1
L64:	cmp eax, 0
L65:	jz L72
L66:	mov eax, [mean]
L67:	sub eax, 60
L68:	mov [mean], eax
L69:	invoke	printf,chr$("Your score is %d"),mean
L70:	invoke	printf,chr$(" higher than 60!",0dh,0ah)
L71:	jmp L78
L72:	mov eax, 60
L73:	sub eax, [mean]
L74:	mov [mean], eax
L75:	invoke	printf,chr$("Your score is %d"),mean
L76:	invoke	printf,chr$(" lower than 60!",0dh,0ah)
L77:	invoke	printf,chr$("!",0dh,0ah)
L78:	exit
main ENDP
END main