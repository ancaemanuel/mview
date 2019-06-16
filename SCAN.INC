function scan_str(segment:word; var str:string; var offs_match:word):boolean;assembler;
var
  lungime:byte;
  ofs_str:word;
  sav_si :word;
asm
  push ds
  les di,str
  mov al,es:[di]
  mov lungime,al
  xor dx,dx
  mov dl,al
  inc di
  mov ofs_str,di
  mov ax,segment
  mov ds,ax
  xor si,si
  cld
  mov cx,$FFF0
@restore:
  mov bl,es:[di]
  inc di
  dec dl
  xor bl,$55
  and bl,$DF
@continua:
  lodsb
  and al,$DF
  cmp al,bl
  jz @found
  loop @continua
@out:
  xor ax,ax
  jmp @end
@found:
  mov sav_si,si
  or dl,dl
  jz @match
  mov bl,es:[di]
  xor bl,$55
  and bl,$DF
  inc di
  dec dx
  lodsb
  and al,$DF
  cmp al,bl
  jz @found
  mov di,ofs_str
  mov si,sav_si
  xor dx,dx
  mov dl,lungime
  jmp @restore
@match:
  mov ax,si
  sub al,lungime
  sbb ah,0
  les di,offs_match
  stosw
@end:
  pop ds
end;

procedure XorStr(var str:string);assembler;
asm
  push ds
  lds si,str
  les di,str
  cld
  lodsb
  inc di
  xor cx,cx
  mov cl,al
@1:
  lodsb
  xor al,$55
  stosb
  loop @1
  pop ds
end;

procedure SecureReadStr(var str:string);
var
  count:byte;
  key:char;
begin
  for count:=0 to 255 do str[count]:=#0;
  count:=0;
  repeat
    key:=readkey;
    if key=#27 then begin str:=''; exit; end;
    if (key in [#32..#126]) and (count<20)
    then begin
      inc(count);
      str[0]:=char(count);
      str[count]:=key;
      write(key);
    end else if (key=#8) and (count>0) then begin
      dec(count);
      str[0]:=char(count);
      write(#8,'ש',#8);
    end else if key<>#13 then write(#7);
  until key=#13;
  XorStr(str);
end;

procedure add_dw_w(var x:dword; source:word);assembler;
asm
  mov ax,source
  les di,x
  add es:[di],ax
  jnc @exit
  inc word ptr es:[di+2]
@exit:
end;

procedure scan;
var x,y,z:byte;
    b:dword;
    ofs:word;
    s:string[22];
begin
  b.h:=0;
  x:=wherex; y:=wherey;
  b.l:=p+1;
  if not again then begin
    b.l:=p;                                  { incepe cu paragraful curent }
    scr_fill(176,scr_attr);
    s:='Enter a string:'; scr_write(28,10,s);
    s:='[שששששששששששששששששששש]'; scr_write(28,12,s);

    gotoxy(29,12);
    SecureReadStr(str);
  end;
  if str<>'' then begin
    scr_fill($20,scr_attr);
    s:='Searching...'; scr_write(33,12,s);
    while b.h=0 do begin
      if scan_str(b.l,str,ofs) then begin
        p := b.l + ofs div 16;
        x :=  64 + ofs mod 16;
        ZipSound($210,$114);
        gotoxy(x,0); exit;
      end;
      add_dw_w(b,$ff0);
    end;
    gotoxy(33,12); write('NOT FOUND ! '); waitkey;
  end;
  gotoxy(x,y);
end;
