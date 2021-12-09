#
# sum_vec1.s
#	(revised on 2020-05-02)
#
	INITIAL_GP = 0x10008000		# initial value of global pointer
	INITIAL_SP = 0x7ffffffc		# initial value of stack pointer
# system call service number
	stop_service = 99

	.text						# テキストセグメントの開始
init:
# initialize $gp (global pointer) and $sp (stack pointer)
	la		$gp, INITIAL_GP		# $gp <-- 0x10008000 (INITIAL_GP)
	la		$sp, INITIAL_SP		# $sp <-- 0x7ffffffc (INITIAL_SP)
	jal		main				# jump to `main`
	nop							# (delay slot)
	li		$v0, stop_service	# $v0 <-- 99 (stop_service)
	syscall						# stop
	nop
# not reach here
stop:							# if syscall return
	j 		stop				# infinite loop...
	nop							# (delay slot)

	.text 	0x00001000			# 以降のコードを0x00001000から配置
main:
    la		$t1, N          	# N の先頭アドレス
    lw		$t2, 0($t1)         # N: データの個数

    ori     $t3, $zero, 0	    # 計算結果の初期値

    la		$t1, BASE          	# BASE の先頭アドレス
    lw		$t4, 0($t1)         # BASE: データの格納されている先頭アドレス
	nop

_LOOP:
	beq		$t2, $zero, _END	# 終了判定
	nop
	lw      $t5, 0($t4)			# メモリからロード

	# ↓$t5 を使う↓ #
	add	    $t3, $t3, $t5   	# 加算結果格納用のレジスタを更新
	# ↑$t5 を使う↑ #

    addi    $t4, $t4, 4			# 先頭アドレスからの位置を更新
    addi    $t2, $t2, -1		# カウンタを減少
	j		_LOOP
	nop

_END:
    la		$s0, RESULT         # RESULT: 加算結果格納用
	sw      $t3, 0($s0)  		# データをストア
	jr		$ra					# リターン
	nop							# (delay slot)

#
# data segment
#
	.data	0x10004000			# データセグメントの開始
RESULT:	.word	0xffffffff		# ここに計算結果を格納する
BASE:	.word	0x10004100		# 加算するデータが格納されている先頭アドレス
N:	.word	10               	# n の初期値

	.data	0x10004100
.word	1, 2, 3, 4, 5, 6, 7, 8, 9, 10

# End of file (sum_vec1.s)
