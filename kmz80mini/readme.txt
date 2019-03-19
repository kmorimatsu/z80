********************************************
*                                          *
*           KM-Z80 mini ver 0.3            *
*            Katsumi Morimatsu             *
*     kmorimatsu@users.sourceforge.jp      *
* http://hp.vector.co.jp/authors/VA016157/ *
*                                          *
********************************************

KM-Z80 miniは、１チップで動作する、MZ-80K互換マイコンです。

０）免責事項

このツールを参考にして行った事による、いかなる不具合に関しても、私、Katsumi
は責任を持ちません。各自、自己責任のもとで作業を行ってください。


１）著作権について

cgrom.hは、"MZ-700 Emulator MZ700WIN For Windows"に付属のフォントデータを
利用して作成してあります。従って、このファイルについては、"Takeshi Maruyama"
さんが著作権を所有していることになります。

また、monitorrom.hについては、同じく"MZ-700 Emulator MZ700WIN For Windows"用の
"MZ-NEW MONITOR"のコードをもとに作成しました。著作権は武蔵野マイコンクラブが
所有しています。

これら、２つのファイルの著作権に関しては、2013年6月10日現在、次のwebページに
情報があります。
http://retropc.net/mz-memories/mz700/

その他のソースコードは、LGPL ver 2.0とします。再配布の際は、これらの著作権に
従った形で行ってください。

バイナリファイルの再配布も、これらの著作権に従って行ってください。著作権につ
いて疑問や質問がある場合は、私、Katsumiに連絡してくだされば幸いです。

２）配布物に含まれているフォルダとファイル

・kmz80miniフォルダ（ソースファイルを含む）
・license.txt（LGPLライセンス）
・readme.txt（今読んでいるファイル）
・kmz80mini.pdf（回路図）
・kmz80mini.hex（バイナリ）

３）KM-Z80 miniの作成と実行に必要なもの

ICについては、次のものを使用しています。

・PIC32MX150F128B (Microchip microcontroller; 50 MHzで動かせるロットが望ましい)

その他の半導体は、次のものを使用しています。

・3.3V　３端子レギュレーター（100 mA 流せればよい）
・緑/赤　２色LED

その他、抵抗・コンデンサー・水晶発振子などを使っています。

回路図に記載されている部品以外に、Microchip社のPICマイコン書き込み機、PICkit3が
必要です。

また、PS/2キーボードとNTSCのビデオ信号を認識するテレビモニターが必要です。

４）注意点

発振回路については、回路のちょっとした違いで発振したりしなかったり、安定しな
かったりするようです。このあたりは、私も手探りの状態です。添付の回路図は、手元
の環境で動作を確認していますが、同じ回路を組んでもうまく発振しないこともあるよ
うです。そういう場合は、接続されている水晶発振子とコンデンサーをはずし、９番ピ
ンに、別途用意した4 Mhzの発振器からの信号を入れるようにすると、うまくいくかも
しれません。ただし、ver 0.3以降では、それ以前のバージョンに無かった水晶発振の
停止を防止するルーチンが追加されていますので、比較的安定に動作するとは思います。

水晶発振停止防止ルーチンは、緑/赤LEDを利用しています。回路の緑/赤LEDを省略した
時に、水晶発振が停止する不具合が観察される場合、RB4とRB5の間に330Ωの抵抗を挿
入して下さい。

５）使い方

基本的には、MZ-80Kと同様です。メモリーは、$1000-$7FFFの、28 Kbytesが使用可能で
す。なお、電源を入れた状態で、KM-BASIC（整数型BASICインタープリター）がすでに
ロードされています。"GOTO$1200"と入力することで実行可能です。

６）ソースファイルのコンパイルについて

プログラムのコンパイルには、MPLAB IDE v8.84とMPLAB C32 complier v2.02を用いて
います。もし、これらの上位バージョンを利用してのコンパイルに問題が在れば、これ
らのバージョンを用いてみて下さい。なお、最適化は「１」でコンパイルしています。
"Project"プルダウンメニューから"Build Options -> Project"を選択し、"MPLAB
PIC32 C Compiler"タブを表示後、"Categories"に"Optimization"を選択することで、
最適化の程度を設定することが可能です。

７）PS/2キーボードとMZ80Kキーボードの対比

US-ASCII(101)及びJIS(106)キーボードのどのキーがMZ80Kのどのキーに対応するかは、
次の表に従います。

101	106	MZ80K
----------------------
F9	F9	．
F5	F5	゛
F3	F3	を
F1	F1	け
F2	F2	む
F12	F12	分
F10	F10	。
F8	F8	、
F6	F6	「
F4	F4	゜
TAB	TAB	HOME CLR
`	全角	
L ALT	L ALT	↑↓
L SHFT	L SHIFT	L SHIFT
L CTRL	L CTRL	←→
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
'	:	ろ
[	@	=
=	=	秒
CAPS	CAPS	カナ英数
R SHFT	R SHIFT	R SHIFT
ENTER	ENTER	CR
]	[	￡
\	]	￥
BKSP	BKSP	INST DEL
KP 1	KP 1	．
KP 4	KP 4	、
KP 7	KP 7	」
KP 0	KP 0	。
KP .	KP .	ー
KP 2	KP 2	ぇ
KP 5	KP 5	ぅ
KP 6	KP 6	ょ
KP 8	KP 8	ぃ
ESC	ESC	BREAK
NUM	NUM	
F11	F11	時
KP +	KP +	ぉ
KP 3	KP 3	っ
KP -	KP -	ぁ
KP *	KP *	ゃ
KP 9	KP 9	ゅ
F7	F7	」

８）バージョン履歴

2013/07/27      ver 0.3
・水晶発振の周波数を8 MHzから4 MHzに変更
・回路図を若干変更
　１）RA1/RB14への入力をRA3からRB2に変更
　２）Playerへの接続をRB2からRB3に変更
　３）green-LEDの接続をRB3からRB5に変更
　４）PS/2キーボードの接続端子にプルアップ抵抗を追加
・水晶発振の停止を防ぐルーチンをmain.cのtimer1Int()関数に挿入

2013/06/10	ver 0.2
・最初の正式公開バージョン