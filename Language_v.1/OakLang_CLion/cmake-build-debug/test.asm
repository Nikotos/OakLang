
	section .data
		 @@dict dw '0123456789ABCDEFD'
		 @@space db 0xD, 0xA
		 @@space_size equ 2
		 @@output_dec times 6 db 0
		 @@output_dec_len equ $ - @@output_dec
		 @@nop_indent times 20 db 0x90
		 @@buffer times 40 db 0x90
		 @@buffer_len equ $ - @@buffer


SECTION .TEXT
		 	GLOBAL _start


_start:
call $

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
		mov dl, 0
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

	 main:
	 nop
	 ret

	 times 500 db 0x90
