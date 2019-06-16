function GetVideoMode:byte;assembler;
asm
  mov ah,$0f
  int $10
end;

function GetVideoSeg:word;
var
  mode: shortint;
begin
  asm
    mov ah,$0f
    int $10
    mov byte ptr mode,al
  end;
  case mode of
    1..6,8..9 : getvideoseg := $b800;
    -2,7,10   : getvideoseg := $b000;
  else getvideoseg := $a000;
  end;
end;

procedure clrscr;assembler;
asm
  mov ah,15
  int 10h
  mov ah,0
  int 10h
end;

procedure scr_write(x,y:byte; var s:string);assembler;
asm
  mov ax,VideoSeg
  mov es,ax
  xor ax,ax
  mov al,160
  mul y
  xor bx,bx
  mov bl,x
  shl bx,1
  add ax,bx
  mov di,ax
  cld
  push ds
  lds si,s
  xor ax,ax
  lodsb
  mov cx,ax
@1:
  movsb
  inc di
  loop @1
  pop ds
end;

procedure MovScreen(Dest,Source:pointer);assembler;
asm
  push ds
  mov cx,2000
  cld
  lds si,Source
  les di,Dest
  rep movsw
  pop ds
end;

procedure scr_Fill(fill,attr:byte);assembler;
asm
  mov cx,2000
  mov ax,VideoSeg
  mov es,ax
  xor di,di
  mov al,fill
  mov ah,attr
  cld
  rep stosw
end;

procedure scr_CopyCol(scr1,scr2,col1,col2:word);assembler;
asm
  { load segment of screen1 (destination) }
  mov ax,scr1
  mov es,ax
  { load offset }
  mov di,col1
  shl di,1
  { load segment of screen2 (source) }
  mov ax,scr2
  push ds
  mov ds,ax
  { load offset }
  mov si,col2
  shl si,1
  { go copy }
  mov cx,25
  cld
@1:
  movsw
  add si,160-2
  add di,160-2
  loop @1
  pop ds
end;

procedure scr_asmLeftScroll(scr:word);assembler;
asm
  push ds
  mov ax,scr
  mov ds,ax
  mov es,ax
  xor di,di
  mov si,2
  mov cx,25*80-1
  cld
  rep
  movsw
  pop ds
end;

procedure scr_asmRigthScroll(scr:word);assembler;
asm
  push ds
  mov ax,scr
  mov ds,ax
  mov es,ax
  mov di,25*160
  mov si,25*160-2
  mov cx,26*80-1
  std
  rep movsw
  pop ds
end;

procedure Delay(ms:word);
var x:longint;
begin
  { de fapt 55*ms milisecunde }
  x:=meml[0:$46c] + ms;
  repeat until (x <= meml[0:$46c]);
end;

procedure scr_LeftScroll(scr1,scr2:word);
var i,j:word;
begin
  for i:=0 to 80-1 do begin
    scr_asmLeftScroll(scr1);
    scr_CopyCol(scr1,scr2,79,i);
    {Delay(1);{}
  end;
end;

procedure scr_RigthScroll(scr1,scr2:word);
var i:word;
begin
  for i:=80-1 downto 0 do begin
    scr_asmRigthScroll(scr1);
    scr_CopyCol(scr1,scr2,0,i);
  end;
end;
