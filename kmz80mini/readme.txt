********************************************
*                                          *
*           KM-Z80 mini ver 0.3            *
*            Katsumi Morimatsu             *
*     kmorimatsu@users.sourceforge.jp      *
* http://hp.vector.co.jp/authors/VA016157/ *
*                                          *
********************************************

KM-Z80 mini�́A�P�`�b�v�œ��삷��AMZ-80K�݊��}�C�R���ł��B

�O�j�Ɛӎ���

���̃c�[�����Q�l�ɂ��čs�������ɂ��A�����Ȃ�s��Ɋւ��Ă��A���AKatsumi
�͐ӔC�������܂���B�e���A���ȐӔC�̂��Ƃō�Ƃ��s���Ă��������B


�P�j���쌠�ɂ���

cgrom.h�́A"MZ-700 Emulator MZ700WIN For Windows"�ɕt���̃t�H���g�f�[�^��
���p���č쐬���Ă���܂��B�]���āA���̃t�@�C���ɂ��ẮA"Takeshi Maruyama"
���񂪒��쌠�����L���Ă��邱�ƂɂȂ�܂��B

�܂��Amonitorrom.h�ɂ��ẮA������"MZ-700 Emulator MZ700WIN For Windows"�p��
"MZ-NEW MONITOR"�̃R�[�h�����Ƃɍ쐬���܂����B���쌠�͕�����}�C�R���N���u��
���L���Ă��܂��B

�����A�Q�̃t�@�C���̒��쌠�Ɋւ��ẮA2013�N6��10�����݁A����web�y�[�W��
��񂪂���܂��B
http://retropc.net/mz-memories/mz700/

���̑��̃\�[�X�R�[�h�́ALGPL ver 2.0�Ƃ��܂��B�Ĕz�z�̍ۂ́A�����̒��쌠��
�]�����`�ōs���Ă��������B

�o�C�i���t�@�C���̍Ĕz�z���A�����̒��쌠�ɏ]���čs���Ă��������B���쌠�ɂ�
���ċ^��⎿�₪����ꍇ�́A���AKatsumi�ɘA�����Ă�������΍K���ł��B

�Q�j�z�z���Ɋ܂܂�Ă���t�H���_�ƃt�@�C��

�Ekmz80mini�t�H���_�i�\�[�X�t�@�C�����܂ށj
�Elicense.txt�iLGPL���C�Z���X�j
�Ereadme.txt�i���ǂ�ł���t�@�C���j
�Ekmz80mini.pdf�i��H�}�j
�Ekmz80mini.hex�i�o�C�i���j

�R�jKM-Z80 mini�̍쐬�Ǝ��s�ɕK�v�Ȃ���

IC�ɂ��ẮA���̂��̂��g�p���Ă��܂��B

�EPIC32MX150F128B (Microchip microcontroller; 50 MHz�œ������郍�b�g���]�܂���)

���̑��̔����̂́A���̂��̂��g�p���Ă��܂��B

�E3.3V�@�R�[�q���M�����[�^�[�i100 mA ������΂悢�j
�E��/�ԁ@�Q�FLED

���̑��A��R�E�R���f���T�[�E�������U�q�Ȃǂ��g���Ă��܂��B

��H�}�ɋL�ڂ���Ă��镔�i�ȊO�ɁAMicrochip�Ђ�PIC�}�C�R���������݋@�APICkit3��
�K�v�ł��B

�܂��APS/2�L�[�{�[�h��NTSC�̃r�f�I�M����F������e���r���j�^�[���K�v�ł��B

�S�j���ӓ_

���U��H�ɂ��ẮA��H�̂�����Ƃ����Ⴂ�Ŕ��U�����肵�Ȃ�������A���肵��
�������肷��悤�ł��B���̂�����́A������T��̏�Ԃł��B�Y�t�̉�H�}�́A�茳
�̊��œ�����m�F���Ă��܂����A������H��g��ł����܂����U���Ȃ����Ƃ������
���ł��B���������ꍇ�́A�ڑ�����Ă��鐅�����U�q�ƃR���f���T�[���͂����A�X�ԃs
���ɁA�ʓr�p�ӂ���4 Mhz�̔��U�킩��̐M��������悤�ɂ���ƁA���܂���������
����܂���B�������Aver 0.3�ȍ~�ł́A����ȑO�̃o�[�W�����ɖ��������������U��
��~��h�~���郋�[�`�����ǉ�����Ă��܂��̂ŁA��r�I����ɓ��삷��Ƃ͎v���܂��B

�������U��~�h�~���[�`���́A��/��LED�𗘗p���Ă��܂��B��H�̗�/��LED���ȗ�����
���ɁA�������U����~����s����ώ@�����ꍇ�ARB4��RB5�̊Ԃ�330���̒�R��}
�����ĉ������B

�T�j�g����

��{�I�ɂ́AMZ-80K�Ɠ��l�ł��B�������[�́A$1000-$7FFF�́A28 Kbytes���g�p�\��
���B�Ȃ��A�d������ꂽ��ԂŁAKM-BASIC�i�����^BASIC�C���^�[�v���^�[�j�����ł�
���[�h����Ă��܂��B"GOTO$1200"�Ɠ��͂��邱�ƂŎ��s�\�ł��B

�U�j�\�[�X�t�@�C���̃R���p�C���ɂ���

�v���O�����̃R���p�C���ɂ́AMPLAB IDE v8.84��MPLAB C32 complier v2.02��p����
���܂��B�����A�����̏�ʃo�[�W�����𗘗p���ẴR���p�C���ɖ�肪�݂�΁A����
��̃o�[�W������p���Ă݂ĉ������B�Ȃ��A�œK���́u�P�v�ŃR���p�C�����Ă��܂��B
"Project"�v���_�E�����j���[����"Build Options -> Project"��I�����A"MPLAB
PIC32 C Compiler"�^�u��\����A"Categories"��"Optimization"��I�����邱�ƂŁA
�œK���̒��x��ݒ肷�邱�Ƃ��\�ł��B

�V�jPS/2�L�[�{�[�h��MZ80K�L�[�{�[�h�̑Δ�

US-ASCII(101)�y��JIS(106)�L�[�{�[�h�̂ǂ̃L�[��MZ80K�̂ǂ̃L�[�ɑΉ����邩�́A
���̕\�ɏ]���܂��B

101	106	MZ80K
----------------------
F9	F9	�D
F5	F5	�J
F3	F3	��
F1	F1	��
F2	F2	��
F12	F12	��
F10	F10	�B
F8	F8	�A
F6	F6	�u
F4	F4	�K
TAB	TAB	HOME CLR
`	�S�p	
L ALT	L ALT	����
L SHFT	L SHIFT	L SHIFT
L CTRL	L CTRL	����
Q	Q	Q
1	1	1
Z	Z	Z
S	S	S
A	A	A
W	W	W
2	2	2
C	C	C
X	X	X
D	D	D
E	E	E
4	4	4
3	3	3
SPACE	SPACE	SPACE
V	V	V
F	F	F
T	T	T
R	R	R
5	5	S
N	N	N
B	B	B
H	H	H
G	G	G
Y	Y	Y
6	6	6
M	M	M
J	J	J
U	U	U
7	7	7
8	8	8
,	,	,
K	K	K
I	I	I
O	O	O
0	0	0
9	9	9
.	.	.
/	/	/
L	L	L
;	;	;
P	P	P
-	-	-
'	:	��
[	@	=
=	=	�b
CAPS	CAPS	�J�i�p��
R SHFT	R SHIFT	R SHIFT
ENTER	ENTER	CR
]	[	��
\	]	��
BKSP	BKSP	INST DEL
KP 1	KP 1	�D
KP 4	KP 4	�A
KP 7	KP 7	�v
KP 0	KP 0	�B
KP .	KP .	�[
KP 2	KP 2	��
KP 5	KP 5	��
KP 6	KP 6	��
KP 8	KP 8	��
ESC	ESC	BREAK
NUM	NUM	
F11	F11	��
KP +	KP +	��
KP 3	KP 3	��
KP -	KP -	��
KP *	KP *	��
KP 9	KP 9	��
F7	F7	�v

�W�j�o�[�W��������

2013/07/27      ver 0.3
�E�������U�̎��g����8 MHz����4 MHz�ɕύX
�E��H�}���኱�ύX
�@�P�jRA1/RB14�ւ̓��͂�RA3����RB2�ɕύX
�@�Q�jPlayer�ւ̐ڑ���RB2����RB3�ɕύX
�@�R�jgreen-LED�̐ڑ���RB3����RB5�ɕύX
�@�S�jPS/2�L�[�{�[�h�̐ڑ��[�q�Ƀv���A�b�v��R��ǉ�
�E�������U�̒�~��h�����[�`����main.c��timer1Int()�֐��ɑ}��

2013/06/10	ver 0.2
�E�ŏ��̐������J�o�[�W����