function wherex:byte;assembler;
asm
	push	ds
	xor	ax,ax
	mov	ds,ax
	mov	si,462h
	lodsb
	mov	si,450h
	shl	ax,1
	add	si,ax
	lodsb
	pop	ds
end;

function wherey:byte;assembler;
asm
	push	ds
	xor	ax,ax
	mov	ds,ax
	mov	si,462h
	lodsb
	mov	si,450h
	shl	ax,1
	inc	ax
	add	si,ax
	lodsb
	pop	ds
end;

procedure gotoxy(x,y:byte);assembler;
asm
	push	ds
	xor	ax,ax
	mov	ds,ax
	mov	bh,[462h]
	mov	dl,[bp+6]
	mov	dh,[bp+4]
	mov	ah,2
	int	10h
	pop	ds
end;
