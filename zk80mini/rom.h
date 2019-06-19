/********************************
* ZK-80 mini written by Katsumi *
*   This script is released     *
*     under the LGPL v2.1.      *
********************************/

static const unsigned char ROM[]={
0xf3,0xc3,0x3b,0x00,0xc3,0x2c,0x01,0x00,0xc3,0x4b,0x00,0x00,0x00,0x00,0x00,0x00,
0xc3,0xd1,0x83,0x00,0x00,0x00,0x00,0x00,0xc3,0xd4,0x83,0x00,0x00,0x00,0x00,0x00,
0xc3,0xd7,0x83,0x00,0x00,0x00,0x00,0x00,0xc3,0xda,0x83,0x00,0x00,0x00,0x00,0x00,
0xc3,0xdd,0x83,0x00,0x00,0x00,0x00,0x00,0xc3,0xe9,0x00,0x3e,0x90,0xd3,0xfb,0xaf,
0x21,0xe0,0x83,0x06,0x14,0x77,0x23,0x05,0xc2,0x45,0x00,0x31,0xd1,0x83,0xcd,0xa1,
0x01,0xcd,0x16,0x02,0x2a,0xec,0x83,0xeb,0x2a,0xee,0x83,0xd6,0x10,0xca,0xb3,0x00,
0x3d,0xca,0xce,0x00,0x3d,0xca,0x8b,0x00,0x3d,0xca,0x9b,0x00,0x3d,0xca,0x97,0x00,
0x3d,0xca,0x9f,0x00,0x3d,0xca,0xa3,0x00,0x3d,0xca,0xa9,0x00,0xc6,0x17,0xeb,0x29,
0x29,0x29,0x29,0x85,0x6f,0x22,0xec,0x83,0xc3,0x4b,0x00,0x62,0x6b,0x22,0xee,0x83,
0x5e,0x2b,0x66,0x6b,0xc3,0x85,0x00,0x23,0xc3,0x8d,0x00,0x2b,0xc3,0x8d,0x00,0x73,
0xc3,0x97,0x00,0xaf,0xd3,0x9e,0xc3,0x4b,0x00,0x3e,0x01,0xd3,0x9e,0xcd,0xa1,0x01,
0xc3,0x4b,0x00,0x2a,0xee,0x83,0x7c,0x17,0xda,0xbc,0x00,0xe9,0x11,0x4b,0x00,0x21,
0xc8,0x83,0x72,0x2b,0x73,0x22,0xe2,0x83,0x2a,0xee,0x83,0x22,0xe0,0x83,0x2a,0xe2,
0x83,0xf9,0x2a,0xea,0x83,0xe5,0xf1,0x2a,0xe0,0x83,0xe5,0x2a,0xe8,0x83,0x44,0x4d,
0x2a,0xe6,0x83,0xeb,0x2a,0xe4,0x83,0xfb,0xc9,0x22,0xe4,0x83,0xf5,0xe1,0x22,0xea,
0x83,0x60,0x69,0x22,0xe8,0x83,0xe1,0x22,0xe0,0x83,0xeb,0x22,0xe6,0x83,0x21,0x00,
0x00,0x39,0x22,0xe2,0x83,0x3a,0xf2,0x83,0xb7,0xca,0x1f,0x01,0x21,0xf0,0x83,0x7e,
0xbb,0xc2,0xce,0x00,0x23,0x7e,0xba,0xc2,0xce,0x00,0x23,0x35,0xc2,0xce,0x00,0xeb,
0x22,0xee,0x83,0x2a,0xea,0x83,0x22,0xec,0x83,0xc3,0x4b,0x00,0xcd,0x3d,0x01,0xc2,
0x4b,0x00,0xdd,0x22,0xee,0x83,0xfd,0x22,0xec,0x83,0xc3,0x4b,0x00,0x06,0x05,0x10,
0xfe,0x3c,0xaf,0xb0,0xc9,0x57,0x1e,0x03,0x3e,0x76,0x3d,0xc2,0x4a,0x01,0x1d,0xc2,
0x4a,0x01,0x7a,0xc9,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x06,0x04,0x21,0xef,0x83,0x11,0xf4,0x83,0x7e,0x12,0x2b,0x13,0x05,0xc2,0xa9,
0x01,0xc3,0xc0,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x3e,0x0f,0xd3,0xfb,0x21,0xf4,0x83,0x11,0xf8,0x83,0x06,0x04,0x7e,0x1f,0x1f,0x1f,
0x1f,0xcd,0xde,0x01,0x7e,0x23,0xcd,0xde,0x01,0x05,0xc2,0xcc,0x01,0xc9,0xe6,0x0f,
0xe5,0x21,0xeb,0x01,0x85,0x6f,0x7e,0x12,0x13,0xe1,0xc9,0x5c,0x06,0x5b,0x4f,0x66,
0x6d,0x7d,0x27,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0xcd,0x26,0x02,0xda,0x16,0x02,0xcd,0x26,0x02,0xd2,
0x1c,0x02,0xc9,0xc3,0x29,0x02,0xcd,0xea,0x02,0x16,0x08,0x06,0x06,0xcd,0x5c,0x02,
0xc2,0x45,0x02,0x16,0x0a,0x06,0x05,0xcd,0x5c,0x02,0xc2,0x45,0x02,0x16,0x0c,0x06,
0x03,0xcd,0x5c,0x02,0xd0,0x16,0xff,0x14,0x1f,0xda,0x47,0x02,0x1e,0xf8,0x7b,0xc6,
0x08,0x5f,0x78,0x0f,0x47,0xda,0x4e,0x02,0x7b,0x82,0x37,0xc9,0x3e,0x09,0xd3,0xfb,
0x3e,0x0b,0xd3,0xfb,0x3e,0x0d,0xd3,0xfb,0x7a,0xd3,0xfb,0xdb,0xf8,0xfe,0xff,0xc9,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xc3,0x45,0x01,
0xcd,0x45,0x01,0xc3,0x45,0x01,0x00,0x00,0x00,0x00,0xc3,0xe0,0x02,0x00,0x00,0xcd,
0x45,0x01,0xcd,0x45,0x01,0xcd,0x45,0x01,0xcd,0x45,0x01,0xc3,0xe0,0x02,0x00,0x00
};


/* Source code follows.
;This code can be aseembled by the attached htmlfile, z80v09.html, or higher version.

(0000)
di
jp init:
end:
(0004)
jp show_ix_iy:
(0008)
jp main:
(0010)
jp 83d1
(0018)
jp 83d4
(0020)
jp 83d7
(0028)
jp 83da
(0030)
jp 83dd
(0038)
jp step:

init:
// Initialize 8255
// Input from port A, output to ports B and C
// Use 0x92 for getting input from port B
ld a,90
out fb,a
// Fill 0x00 in work area
xor a
ld hl,83e0
ld b,14
loop_init:
  ld (hl),a
  inc hl
  dec b
jp nz,loop_init:

main:
ld sp,83d1
// Show address and data
call showaddrdata:
// Check key input (result will be in A register)
call keyinput:
// In advance, load address and data
// into HL and BC registers
ld hl,(83ec) // data
ex de,hl
ld hl,(83ee) // address
// Check if command key, first.
sub 10
jp z,run_key:
dec a
jp z,ret_key:
dec a
jp z,adrs_key:
dec a
jp z,decr_key:
dec a
jp z,incr_key:
dec a
jp z,write_key:
dec a
jp z,store_key:
dec a
jp z,load_key:
add a,17
// In the case of num key
// Shift 4 bits in data register,
// then add value in A register.
ex de,hl
add hl,hl
add hl,hl
add hl,hl
add hl,hl
add a,l
ld l,a
update_data:
ld (83ec),hl
jp main:

adrs_key:
ld h,d
ld l,e

read:
ld (83ee),hl
ld e,(hl)
dec hl
ld h,(hl)
ld l,e
jp update_data:

incr_key:
inc hl
jp read:

decr_key:
dec hl
jp read:

write_key:
ld (hl),e
jp incr_key:

store_key:
xor a
out 9e,a
jp main:

load_key:
ld a,01
out 9e,a
call showaddrdata:
jp main:

run_key:
ld hl,(83ee)
ld a,h
rla
jp c,user_run_key:
// Simply jump to address in monitor program.
jp (hl)
// Restore all registers and run.
user_run_key:
ld de,main:
ld hl,83c8
ld (hl),d
dec hl
ld (hl),e
ld (83e2),hl
ld hl,(83ee)
ld (83e0),hl

ret_key:
ld hl,(83e2)
ld sp,hl
reg_ret_key:
ld hl,(83ea)
push hl
pop af
ld hl,(83e0)
push hl
ld hl,(83e8)
ld b,h
ld c,l
ld hl,(83e6)
ex de,hl
ld hl,(83e4)
ei
ret
end:

;83E0  PC
;83E2  SP
;83E4  HL
;83E6  DE
;83E8  BC
;83EA  AF
;83F0  Break address
;83F2  Break counter (8 bit)
step:
ld (83e4),hl
push af
pop hl
ld (83ea),hl
ld h,b
ld l,c
ld (83e8),hl
pop hl
ld (83e0),hl
ex de,hl
ld (83e6),hl
ld hl,0000
add hl,sp
ld (83e2),hl
// Check if break mode.
ld a,(83f2)
or a
jp z,show_step:
// Break counter is set.
// Check if PC is equel to break point
ld hl,83f0
ld a,(hl)
cp e
jp nz,ret_key:
inc hl
ld a,(hl)
cp d
jp nz,ret_key:
// OK, PC is equal to break point.
// Decrement counter
inc hl
dec (hl)
jp nz,ret_key:
// Counter is zero. Let's break here.
show_step:
ex de,hl
ld (83ee),hl
ld hl,(83ea)
ld (83ec),hl
jp main:
end:

show_ix_iy:
call z80_or_8080:
jp nz,main:
ld (83ee),ix //Z80
ld (83ec),iy //Z80
jp main:
end:

z80_or_8080:
// Z=1 if CPU is Z80, Z=0 if CPU is 8080
ld b,05
loop_z80_or_8080:
djnz loop_z80_or_8080: //Z80
inc a
xor a
or b
ret
end:

;Wait for 4.5 ms
;17+4+11+4+11+(4+10)*A+4+10+
;((4+10)*256+4+10)*(E-1)+4+10
;=9227 when EA=0x038B
;9217/2048=4.50 (msec)
timer45main:
ld d,a
//in a,9e
//ld e,a  // E=0x03 when 2.048 MHz
//in a,9f // A=0x8B when 2.048 MHz
ld e,03 // E=0x03 when 2.0 MHz
ld a,76 // A=0x76 when 2.0 MHz
loop2_timer45:
dec a
jp nz,loop2_timer45:
dec e
jp nz,loop2_timer45:
ld a,d
ret
end:

///////////////
// Free area //
///////////////

(01a1)
showaddrdata:

;Show the address and data
;83EC data
;83EE address

ld b,04
ld hl,83ef
ld de,83f4
loop_showaddrdata:
  ld a,(hl)
  ld (de),a
  dec hl
  inc de
  dec b
jp nz,loop_showaddrdata:
jp showsegdata:

end:

(01c0)
showsegdata:
;Show the data in display register
;as 7 segment data in LED
;83F4 display register
;83F4: left most
;83F7: right most
ld a,0f // For ND80ZIII, output 0x10 to 0x98
out fb,a
ld hl,83f4
ld de,83f8
ld b,04
loop_showsegdata:
  ld a,(hl)
  rra
  rra
  rra
  rra
  call showsegdatasub:
  ld a,(hl)
  inc hl
  call showsegdatasub:
  dec b
jp nz,loop_showsegdata:
ret
end:

// Write a character (4 bits)
showsegdatasub:
and 0f
push hl
ld hl,segdatatable:
add a,l
ld l,a
ld a,(hl)
ld (de),a
inc de
pop hl
ret

segdatatable:
;Hexadecimal to 7 segment conversion table
5C 06 5B 4F
66 6D 7D 27
7F 6F 77 7C
39 5E 79 71
end:

(0216)
keyinput:

;Key input.  Wait until a key will be pressed.

loop_keyinput:
  call timer_keyinputmain:
jp c,loop_keyinput:  // Detect key up.
loop2_keyinput:
  call timer_keyinputmain:
jp nc,loop2_keyinput: // Detect key down.
ret
end:

(0223)
keyinputmain:
jp skip_keyinputmain:
timer_keyinputmain:
call timer9: // Against chattering.
skip_keyinputmain:

;Key input (returns without input)
;Result will be in A register.
;When not pressed,
;A register will be 0xff,
;and carry flag will be 1.
;0-00, 1-01, 2-02, 3-03,
;4-04, 5-05, 6-06, 7-07,
;8-08, 9-09, A-0A, B-0B,
;C-0C, D-0D, E-0E, F-0F
;RET-11, RUN-10,
;STORE DATA-16, LOAD DATA-17
;ADRS SET-12, READ INCR-14,
;READ DECR-13, WRITE INCR-15

// First line
ld d,08
ld b,06
call keyinputmain0:
jp nz,keyinputmain2:
// Second line
ld d,0a
ld b,05
call keyinputmain0:
jp nz,keyinputmain2:
// Third line
ld d,0c
ld b,03
call keyinputmain0:
ret nc
keyinputmain2:
ld d,ff
loop_keyinputmain2:
  inc d
  rra
jp c,loop_keyinputmain2:
ld e,f8
loop2_keyinputmain2:
  ld a,e
  add a,08
  ld e,a
  ld a,b
  rrca
  ld b,a
jp c,loop2_keyinputmain2:
ld a,e
add a,d
scf
ret

keyinputmain0:
// For TK-80, initialize port C of 8255
ld a,09
out fb,a
ld a,0b
out fb,a
ld a,0d
out fb,a
// For TK-80, output reg D to 0xFB,
// and input from 0xF8,
// then output reg D +1 to 0xFB.
// For ND80ZIII, output reg B to 0x9C,
// and input from 0x9C.
ld a,d
out fb,a
in a,f8
cp ff
ret
end:

///////////////
// Free area //
///////////////

// 027c: Serial out routine
// 02a0: Serial in routine

(02dd)
timer45:
jp timer45main:
end:
timer9main:
call timer45main:
jp timer45main:
end:
(02ea)
// Wait for 9.0 ms
timer9:
jp timer9main:
end:
(02ef)
// Wait for 27 ms
timer27:
call timer45main:
call timer45main:
call timer45main:
call timer45main:
jp timer9main:
nop
nop
end:

//*/