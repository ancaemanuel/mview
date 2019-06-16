{$A+,B-,D-,E-,F-,G-,I+,L-,N-,O-,P-,Q-,R-,S+,T-,V-,X-}
{$M $800,0,0 }

uses dos;
type
  dword=record l,h:word end;

var
  VideoSeg,P,I:WORD;
  str:string;
  again:boolean;
  firstMCB:boolean;
  urmMCB,CurrentIP,DataSeg:word;
  KbdIntVec:Procedure;
  sav_x,sav_y,scr_attr:byte;
  scr_full_mode:boolean;
  Active:boolean;


{$I   VIDEO.INC}
{$I   CUSOR.INC}
{$I     KEY.INC}
{$I    SCAN.INC}
{$I  Memory.INC}


procedure init;
begin
  FirstMCB:=true;
  str:='';
  P:=0;
  scr_full_mode:=false;
  clrscr;
  VideoSeg:=GetVideoSeg;
  scr_attr:=mem[VideoSeg:1];
  Active:=false;
  asm
    call @1
    @1:
    pop ax
    mov cx,4
    shr ax,cl
    mov bx,cs
    add ax,bx
    mov CurrentIP,ax

    mov dx,$1234            { Allready installed ? }
    int $09
    cmp dx,$4321
    jne @NotActive
    mov al,true
    mov Active,al
    @NotActive:
  end;
end;

procedure run;
var
  ExtKey,Key:byte;
  step:word;
BEGIN
  gotoxy(12,0);
  REPEAT
    WritePage;
    extendedkey(ExtKey,Key);
    again:=false;
    if scr_full_mode then begin
      case key of
      27:exit;
      0 :case extkey of
         80: p:=p+5;                           { DN }
         72: p:=p-5;                           { UP }
         73: p:=p-125;                         { PgUp }
         81: p:=p+125;                         { PgDn }
         end;
      32:scr_full_mode:=false;
      end;
    end else begin
    CASE Key OF
      27: EXIT;
      0 : CASE ExtKey OF
        80 : INC(P);                            { DN }
        72 : DEC(P);                            { UP }
        77 : right;
        75 : left;
        71 : P:=0;                              { HOME  }
        79 : P:=$FFE7;                          { End   }
        73 : P:=P-$19;                          { PgUp  }
        81 : P:=P+$19;                          { PgDn  }
        132: P:=P-$190;                         { ^PgUp }
        118: P:=P+$190;                         { ^PgDn }
        68 : if active then begin               { F10   }
               clrscr;
               setintvec($09,Addr(KbdIntVec));
               halt;
             end;
      END;
      48..57,65..70,97..102:
        if key<58 then P:=(key-48)*$1000
        else P:=((key and $DF)-55)*$1000;
      83,115: scan;                             { S     }
      71,103: begin again:=true; scan; end;     { G     }
      23    : writemsg;                         { ^W    }
      09    : tab;
      73,105: Interrupts;
      77,109: Mcb;
      84,116: DTA;
      32    : scr_full_mode:=true;
      36    : asm mov ax,sp; shr ax,1; shr ax,1; shr ax,1; shr ax,1;
                  mov bx,ss; add ax,bx; mov p,ax
              end;
      64    : P:=CurrentIP;
      35    : P:=DataSeg;
      END;
    end;
  UNTIL FALSE;
END;

procedure done;
begin
  clrscr;
  gotoxy(21,12);
  WriteLn('Memory View 1.7 (c) 93,98 by EmiSoft');
end;

{$F+}
procedure Activate; interrupt;
var flags:word;
begin
  asm
    pushf
    pop ax
    mov flags,ax
  end;

  inline ($9C); { PUSHF }
  KbdIntVec;
  asm
    mov dx,[BP+10]             { Self test }
    cmp dx,$1234
    jne @1
    mov dx,$4321
    mov [BP+10],dx
    @1:
  end;
  if Active then exit;
  if (flags and $30) <> 0 then exit;

  if (mem[0:$417] and $03) <> $03       { not (Left and Right Shift) ? }
  then exit;

  {init}
    Active:=true;
    asm
      mov ax,[BP+18]
      mov cx,4
      shr ax,cl
      add ax,[BP+20]
      mov CurrentIP,ax
      mov ax,[BP+4]
      mov DataSeg,ax
    end;
    sav_x:=wherex; sav_y:=wherey;
    MovScreen( ptr(VideoSeg+$0700,0), ptr(VideoSeg,0) );

    VideoSeg:=VideoSeg+$0600;
    scr_Fill($20,scr_attr);
    WritePage;
    VideoSeg:=VideoSeg-$0600;

    scr_RigthScroll(VideoSeg,VideoSeg+$0600);
  {end init}

  run;
  gotoxy(sav_x,sav_y);
  scr_LeftScroll(VideoSeg,VideoSeg+$0700);
  Active:=false;
end;
{$F-}

BEGIN
  init;
  if not Active and (paramstr(1)='tsr') then begin
    GetIntVec($9,@KbdIntVec);
    SetIntVec($9,Addr(Activate));
    Keep(0); { Terminate, stay resident }
  end;
  run;
  done;
END.
