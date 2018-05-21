
	section .data
		 @@buffer times 40 db 0
		 @@buffer_len equ $ - @@buffer
		 @@dict dw '0123456789ABCDEFD'
		 @@space db 0xD, 0xA
		 @@space_size equ 2
		 @@output_dec times 6 db 0
		 @@output_dec_len equ $ - @@output_dec
		 @@nop_indent times 20 db 0x90

SECTION .TEXT
		GLOBAL _start


_start:
call main

mov eax, 1
mov ebx, 0
int 80h


input:
	mov eax, 3
  mov ebx, 0
  mov ecx, @@buffer
  mov edx, @@buffer_len
  int 80h

	mov r15d, @@buffer
	xor rax, rax
	xor rbx, rbx
	xor rdx, rdx
	xor rcx, rcx
	call @@to_reg
	ret

@@to_reg:
	mov ebx, 10
	mul ebx
	mov cl, [r15d]
	cmp cl, '0'
	jl @@exit
	cmp cl, '9'
	jg @@exit
	sub cl, '0'
	add ax, cx
	add r15d, 1
	jmp @@to_reg
	@@exit:
	mov bx, 10
	xor rdx, rdx
	div bx
	ret

output:
	mov r13, [rsp + 8]
	call @@clean_dec_buff
	call @@Print_Dec
	call @@Print_space
	ret


	@@Print_Dec:
		mov eax, r13d
		mov r12d, 0
		mov r11d, 100000

		@@cycle1:
			mov rdx, 0
			mov ecx, 10
			div ecx

			mov r14, [@@dict + edx]
			push r14

			add r12d, 1
			cmp r12d, 6
		jne @@cycle1

		mov r12d, 0
		@@cycle2:
			pop r14
			mov [@@output_dec + r12d], r14d
			add r12d, 1
		cmp r12d, 6
		jne @@cycle2


		mov r8d, @@output_dec
		mov edx, @@output_dec_len
		call @@print_to_console
	ret


@@clean_dec_buff:
	mov ecx, @@output_dec_len
	@@clean_loop:
		sub ecx, 1
		mov dl, 0x0
		mov [@@output_dec + ecx], dl
		cmp ecx, 0
	jne @@clean_loop
	ret


@@print_to_console:
	mov eax, 4
	mov ebx, 1
	mov ecx, r8d
	int 80h
ret

	@@Print_space:
		mov eax, 4
		mov ebx, 1
		mov ecx, @@space
		mov edx, @@space_size
		int 80h
	ret

det:
push rbp
mov rbp, rsp
sub rsp, 256
mov rax, [rbp + 24]
push rax
mov rax, [rbp + 32]
push rax
pop rax
pop rbx
mul ebx
push rax
mov rax, [rbp + 16]
push rax
mov rax, [rbp + 40]
push rax
pop rax
pop rbx
mul ebx
push rax
pop rax
pop rbx
sub rax, rbx
push rax
pop rax
add rsp, 256
pop rbp
ret

solve:
push rbp
mov rbp, rsp
sub rsp, 256
mov rax, [rbp + 32]
push rax
mov rax, [rbp + 48]
push rax
mov rax, [rbp + 40]
push rax
mov rax, [rbp + 32]
push rax
call det
pop rbx
pop rbx
pop rbx
pop rbx
push rax
mov rax, rbp
sub rax, 8
push rax
pop rax
pop rbx
mov [rax], rbx
mov rax, [rbp - 8]
push rax
push 0
pop rax
pop rbx
cmp rax, rbx
jne if_pass_0
push 228
call output
pop rbx
push 228
pop rax
if_pass_0:
mov rax, [rbp - 8]
push rax
push 0
pop rax
pop rbx
cmp rax, rbx
je if_pass_1
mov rax, [rbp - 8]
push rax
mov rax, [rbp + 32]
push rax
mov rax, [rbp + 24]
push rax
mov rax, [rbp + 40]
push rax
mov rax, [rbp + 16]
push rax
call det
pop rbx
pop rbx
pop rbx
pop rbx
push rax
pop rax
pop rbx
xor rdx, rdx
idiv ebx
push rax
call output
pop rbx
mov rax, [rbp - 8]
push rax
mov rax, [rbp + 24]
push rax
mov rax, [rbp + 48]
push rax
mov rax, [rbp + 16]
push rax
mov rax, [rbp + 32]
push rax
call det
pop rbx
pop rbx
pop rbx
pop rbx
push rax
pop rax
pop rbx
xor rdx, rdx
idiv ebx
push rax
call output
pop rbx
if_pass_1:
add rsp, 256
pop rbp
ret

main:
push rbp
mov rbp, rsp
sub rsp, 256
call input
push rax
call input
push rax
call input
push rax
call input
push rax
call input
push rax
call input
push rax
call solve
pop rbx
pop rbx
pop rbx
pop rbx
pop rbx
pop rbx
add rsp, 256
pop rbp
ret

