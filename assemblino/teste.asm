; Teste de programação do Arduino "assemblando" manualmente os opcodes
; DDRD = 0a
; PORTD = 0b
; Os opcodes precisam ser convertidos para little-endian no .bin
addr	opcode	instruction
0000	ef0f	ldi r16, 0xff	; programa porta D como saídas
0001	b90a	out DDRD, r16
0002	e000	ldi r16, 0x00	; limpa registradores de contagem dos delays
0003	e010	ldi r17, 0x00
0004	e440	ldi r20, 0x40	; inicio
0005	b94b	out PORTD, r20	; loop1, led para a direita
0006	e024	ldi r18, 0x04
0007	9503	inc r16		; delay1
0008	f7f1	brne delay1
0009	9513	inc r17
000a	f7e1	brne delay1
000b	952a	dec r18
000c	f7d1	brne delay1
000d	9546	lsr r20
000e	f7b1	brne loop1
000f	e042	ldi r20, 0x02
0010	b94b	out PORTD, r20	; loop2, led para a esquerda
0011	e024	ldi r18, 0x04
0012	9503	inc r16		; delay2
0013	f7f1	brne delay2
0014	9513	inc r17
0015	f7e1	brne delay2
0016	952a	dec r18
0017	f7d1	brne delay2
0018	0f44	lsl r20
0019	f7b1	brne loop2
001a	cfe5	rjmp inicio

