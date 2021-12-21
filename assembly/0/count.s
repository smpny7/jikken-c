#
# count.s
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
    lw		$t2, 0($t1)         # $t2: 足し算の繰り返し回数

    ori     $t3, $zero, 0	    # $t3: 計算結果

_LOOP:
	beq		$t2, $zero, _END	# 終了判定
	nop

	add	    $t3, $t3, $t5   	# 加算結果格納用のレジスタを更新

    add		$t3, $t3, $t2		# $t3 += $t2
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
N:	.word	10               	# n の初期値

# End of file (sum_vec1.s)
