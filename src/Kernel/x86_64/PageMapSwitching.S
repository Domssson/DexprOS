.text
    .global DexprOS_Is5LevelPagingActive


DexprOS_Is5LevelPagingActive:
    /* Check bit 12 of CR4 (LA57) */
    mov %cr4, %rax
    test $0x1000, %rax
    jnz .5_level_paging_active
    
    mov $0, %rax
    ret
.5_level_paging_active:
    mov $1, %rax
    ret

