.data
b1:     .byte   5
h1:     .half   300
w1:     .word   1000
w2:     .word   0

.text
main:
    # Load immediates / addresses
    li   $t0, 10
    li   $t1, 3
    la   $t2, w1

    # Load from memory
    lw   $t3, 0($t2)      # t3 = w1
    lb   $t4, b1
    lh   $t5, h1

    # Arithmetic
    add  $t6, $t3, $t0
    sub  $t6, $t6, $t1
    addu $t7, $t6, $t1
    subu $t7, $t7, $t1

    # Logical
    and  $s0, $t6, $t0
    or   $s1, $t6, $t0
    xor  $s2, $t6, $t0
    nor  $s3, $t6, $t0

    # Shifts
    sll  $s4, $t0, 2
    srl  $s5, $s4, 1
    sra  $s6, $s4, 1
    sllv $s7, $t0, $t1

    # Multiply / divide
    mult  $t0, $t1
    mflo  $a0
    mfhi  $a1

    div   $t0, $t1
    mflo  $a2
    mfhi  $a3

    # Store result
    la   $t2, w2
    sw   $a2, 0($t2)

    # Branch tests
    beq  $t0, $t1, equal
    bne  $t0, $t1, notequal

equal:
    li   $v0, 1
    j    after_branch

notequal:
    li   $v0, 2

after_branch:
    # Pseudo branches
    blt  $t1, $t0, less
    bge  $t0, $t1, greater_equal

less:
    li   $v1, 3
    j    call_func

greater_equal:
    li   $v1, 4

call_func:
    jal  func
    move $s0, $v0

end:
    j end

# ---------------------
func:
    addi $sp, $sp, -4
    sw   $ra, 0($sp)

    li   $v0, 42

    lw   $ra, 0($sp)
    addi $sp, $sp, 4
    jr   $ra