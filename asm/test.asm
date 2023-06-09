bits 16

; MOD 00 | RM 110 => 4 bytes
mov bx, [65535]

; W 0 => 2 bytes
mov cx, 12

; W 1 => 3 bytes
; ignore sign
mov dx, 3948
mov dx, -3948

; MOD 00 => 2 bytes
mov al, [bx + si]

; MOD 01 => 3 bytes
mov ah, [bx + si + 4]

; MOD 10 => 4 bytes
mov al, [bx + si + 4999]

mov al, [bx + si]
mov bx, [bp + di]
mov dx, [bp]

mov [bx + di], cx
mov [bp + si], cl
mov [bp], ch