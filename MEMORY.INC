procedure WritePara(source,dest:pointer);assembler;
asm
  cld
  push ds
  lds  si,source
  mov  dx,ds
  mov  bx,si
  les  di,dest
  mov  ax,es
  mov  al,'['
  stosb
  inc  di
  shl  bx,1
  shl  bx,1
  shl  bx,1
  shl  bx,1
  add  dx,bx
  mov  al,'0'
  stosb
  inc  di
  stosb
  inc  di
  stosb
  inc  di
  mov  al,dh
  call @store
  mov  al,dl
  call @store
  mov  al,'0'
  stosb
  inc  di
  mov  al,']'
  stosb
  inc  di
  mov  cx,$0010
@hexpara:
  mov  dx,cx
  mov  al,' '
  stosb
  inc  di
  shr  dx,1
  jc   @unspatiu
  shr  dx,1
  jc   @unspatiu
  stosb
  inc  di
@unspatiu:
  lodsb
  call @store
  loop @hexpara
  mov  al,' '
  stosb
  inc  di
  stosb
  inc  di
  sub  si,$10
  mov  cx,$0010
@asciipara:
  movsb
  inc  di
  loop @asciipara
  jmp  @return
@store:
  mov  ah,al
  shr  al,1
  shr  al,1
  shr  al,1
  shr  al,1
  call @qbit
  mov  al,ah
  and  al,$0f
@qbit:
  add  al,'0'
  cmp  al,'9'
  jbe  @cifra
  add  al,$07
@cifra:
  stosb
  inc  di
  ret
@return:
  pop ds
end;

procedure asmWriteFull(source:word);assembler;
asm
  mov ax,VideoSeg
  mov es,ax
  xor di,di
  push ds
  mov ax,source
  mov ds,ax
  xor si,si
  mov cx,80*25
  cld
@1:
  movsb
  inc di
  loop @1
  pop ds
end;

procedure WritePage;
var i:word;
begin
  if not scr_full_mode
  then begin
    for i:=0 to 24 do
    WritePara( Ptr(P+I,0), Ptr(VideoSeg,I*160) );
  end
  else asmWriteFull(P);
end;

procedure Mcb;
var x:word;
    id:char;
begin
  if (firstMCB) or (urmMCB=0) then begin
    firstMCB:=false;
    asm
      mov ah,$52
      int $21
      mov ax,es:[bx-2];
      mov x,ax
    end;
    P:=x;
    asm
      mov ax,x
      mov es,ax
      mov ax,es:[$0003]
      mov x,ax
    end;
    urmMCB:=P+x+1;
    exit;
  end;
  P:=urmMCB;
  x:=urmMCB;
  asm
    push ds
    mov ax,x
    mov ds,ax
    mov ax,[$0003]
    add x,ax
    mov al,[$0000]
    mov id,al
    pop ds
  end;
  urmMCB:=x+1;
  if upcase(id)='Z' then urmMCB:=0;
end;

procedure DTA;
var x:word;
begin
  asm
    mov ah,$2f
    int $21
    shr bx,1
    shr bx,1
    shr bx,1
    shr bx,1
    mov ax,es
    add ax,bx
    mov x,ax
  end;
  P:=x;
end;

procedure writeInt(x,y:byte;i:word);
const h:array[0..15] of char = '0123456789ABCDEF';
var v,w:word;
begin
  gotoxy(x,y);
  write('Int',h[i shr 4],h[i and $0f],' ');
  v:=memw[$0:i*4];
  w:=memw[$0:i*4+2];
  write(h[hi(w) shr 4],h[hi(w) and $0f],
        h[lo(w) shr 4],h[lo(w) and $0f],':',
        h[hi(v) shr 4],h[hi(v) and $0f],
        h[lo(v) shr 4],h[lo(v) and $0f]);
  writeln;
end;

procedure Interrupts;
var i:word;
    x,y:byte;
    p:word;
begin
  x:=0; y:=0;
  scr_Fill($20,scr_attr);
  gotoxy(0,0);
  write('Intreruperi suspecte:');
  for i:=0 to $90 do begin
    p:=memw[0:i*4+2];
    p:=p + memw[0:i*4] div 16;
    if (p>0) and ((p<$0052) or (p>$8000)) and (p<$C800)
    then begin
      inc(y);
      if y>22 then begin x:=x+17; y:=1; end;
      writeInt(x,y,i);
    end;
  end;
  gotoxy(70,0); write('Ram ',memw[0:$413],'K');
  gotoxy(0,23); write('Press any key to continue...'); waitkey;
  gotoxy(12,0);
end;

procedure left;
var x:byte;
begin
  x:=wherex;
  if x>12 then
  if x>64 then gotoxy(wherex-1,0)
  else begin
    if ((x+1) mod 13)=0
    then gotoxy(x-3,0)
    else if (((x+1) mod 13) mod 3)=0
         then gotoxy(x-2,0)
         else gotoxy(x-1,0)
  end;
end;

procedure right;
var x:byte;
begin
  x:=wherex;
  if x<79 then
  if x>63 then gotoxy(wherex+1,0)
  else begin
    if ((x-9) mod 13)=0
    then gotoxy(x+3,0)
    else if ((((x-9) mod 13)-1) mod 3)=0
	 then gotoxy(x+2,0)
	 else gotoxy(x+1,0)
  end;
end;

procedure tab;
var x:byte;
begin
  x:=wherex;
  if x<64 then begin
    x:=64+4*((x-12) div 13)+((x-12) mod 13) div 3;
  end else begin
    x:=12+((x-64) div 4)*13+((x-64) mod 4)*3;
  end;
  gotoxy(x,0);
end;

procedure writetoram(c:byte);
var x,d,i,j:byte; ofs:word;
begin
  x:=wherex;
  if x<64 then begin
    if ((c>=48) and (c<=57)) or
       ((c>=65) and (c<=70)) or
       ((c>=97) and (c<=102)) then begin
    if c<58 then i:=c-48 else i:=(c and $DF)-55;
    ofs:=4*((x-12) div 13)+((x-12) mod 13) div 3;
    d:=((x-12) mod 13) mod 3;
    j:=byte(ptr(p,ofs)^);
    if d=0
    then byte(ptr(p,ofs)^):=(j or $F0) and ((i shl 4) or $0F)
    else byte(ptr(p,ofs)^):=(j or $0F) and (i or $F0);
    if j<>byte(ptr(p,ofs)^) then
    if x=61 then begin
      inc(p); gotoxy(12,0);
    end else right;
  end
  end else begin
  byte(ptr(p,x-64)^):=c;
  if byte(ptr(p,x-64)^)=c then
  if x=79 then begin
    inc(p); gotoxy(64,0);
  end else right;
  end;
end;

procedure writemsg;
var x,y:byte;
    write_mode:boolean;
begin
  x:=wherex; y:=wherey;
  scr_Fill($20,scr_attr);
  gotoxy(30,12); write('I LIKE HACKING !'); WaitKey;
  gotoxy(x,y);
  write_mode:=true;
  repeat
    WritePage;
    extendedkey(x,y);
    case y OF
      27: write_mode:=false;
      09: tab;
      00: case x of
        80 : INC(P);       {DN}
        72 : DEC(P);       {UP}
        77 : right;
        75 : left;
        73 : P:=P-$19;     {PgUp}
        81 : P:=P+$19;     {PgDn}
        132: P:=P-$190;    {^PgUp}
        118: P:=P+$190;    {^PgDn}
      end;
      else writetoram(y);
    end;
  until not write_mode;
  x:=wherex; y:=wherey;
  scr_Fill($20,scr_attr);
  gotoxy(28,12); write('Back to read only mode'); waitkey;
  gotoxy(x,y);
end;

procedure MemoryMap;
begin
  scr_Fill($20,scr_attr);

  waitkey;
end;
