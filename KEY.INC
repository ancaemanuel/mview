procedure waitkey;assembler;
asm
  push ds
  xor ax,ax
  mov ds,ax
  mov al,[$041a]
  mov [$041c],al
  pop ds
  int $16
end;

function keypressed:boolean;assembler;
asm
  push ds
  xor ax,ax
  mov ds,ax
  mov al,[41ah]
  cmp al,[41ch]
  jz @0
  mov al,1
  jmp @1
@0:
  mov al,0
@1:
  pop ds
end;

function readkey:char;assembler;
asm
  mov ah,0
  int 16h
end;

procedure extendedkey(var e,k:byte);assembler;
asm
  mov ah,0
  int 16h
  les di,[bp+4]
  stosb
  les di,[bp+8]
  mov al,ah
  stosb
end;

procedure ZipSound(zip,final:word);assembler;
asm
	IN	AL,61h
	OR	AL,0003
	OUT	61h,AL
	MOV	AL,0090h
	OUT	43h,AL
	MOV	AX,0002
@1:	OUT	42h,AL
	MOV	CX,zip       {0210h}
	SUB	CX,AX
@2:	loop	@2
	ADD	AL,02
	JNZ	@1
	SUB	AL,04
@3:	OUT	42h,AL
	MOV	CX,final     {0114h}
@4:	loop	@4
	SUB	AL,4
	JNZ	@3
	IN	AL,61h
	AND	AL,0FCh
	OUT	61h,AL
end;
