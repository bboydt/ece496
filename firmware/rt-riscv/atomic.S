// a0 - address
// a1 - val
//.global __atomic_exchange_explicit
//__atomic_exchange_explicit:
//    fence
//0:  lr.w t0, (a0)
//    sc.w t1, a1, (a0)
//    bne zero, t1, 0b
//    mv a0, t0
//    fence
//    ret
//
// a0 - address
// a1 - val
//.global __atomic_fetch_add_explicit
//__atomic_fetch_add_explicit:
//    fence
//0:  lr.w t0, (a0)
//    add a1, a1, t0
//    sc.w t1, a1, (a0)
//    bne zero, t1, 0b
//    mv a0, t0
//    fence
//    ret
//
