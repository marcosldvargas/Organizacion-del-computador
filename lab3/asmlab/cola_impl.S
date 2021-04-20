#include "cola_asm.h"

.global cola_crear
.global cola_destruir
.global cola_encolar
.global cola_desencolar
.global cola_esta_vacia
.global cola_ver_primero


cola_crear:               
  movl $1, %edi
  movl $SIZEOF_COLA, %esi
  call calloc  
  ret


cola_esta_vacia:          
  cmpq $0, OFFSET_HEAD(%rdi)
  sete %al
  ret


cola_ver_primero:         
  xorq %rax, %rax
  movq OFFSET_HEAD(%rdi), %rcx
  cmpq %rax, %rcx
  je .EXIT
  movq OFFSET_DATA(%rcx), %rax

.EXIT:
  ret


cola_encolar:    
  xorq %rax, %rax
  pushq %rdi
  pushq %rsi
  movl $SIZEOF_NODO, %esi
  movl $1, %edi
  call calloc
  popq %rsi
  popq %rdi
  movq %rsi, OFFSET_DATA(%rax) 
  movq $OFFSET_HEAD, %r8          //Se usa la instrucción leaq para movernos entre direcciones de memoria que serán escritas
  leaq (%rdi, %r8), %rbx
  movq OFFSET_TAIL(%rdi), %rdx
  movq $OFFSET_NEXT, %r9
  leaq (%rdx, %r9), %rdx
  cmpq $0, (%rbx)
  cmovnz %rdx, %rbx
  movq %rax, (%rbx) 
  movq %rax, OFFSET_TAIL(%rdi)
  ret

cola_desencolar:           
  cmpq $0, OFFSET_HEAD(%rdi)
  je .SALIR
  movq OFFSET_HEAD(%rdi), %rdx
  movq OFFSET_DATA(%rdx), %rcx
  movq OFFSET_NEXT(%rdx), %r8
  movq %r8, OFFSET_HEAD(%rdi)
  movq %rdx, %rdi
  pushq %rcx
  call free
  popq %rcx
  movq %rcx, %rax

.SALIR: 
  ret

cola_destruir:             
  jmp .TEST

.LOOP:
  pushq %rdi
  movq OFFSET_HEAD(%rdi), %rdi  
  movq OFFSET_NEXT(%rdi), %rsi
  call free
  popq %rdi
  movq %rsi, OFFSET_HEAD(%rdi)

.TEST:
  cmpq $0, OFFSET_HEAD(%rdi)
  jne .LOOP
  call free
  ret









