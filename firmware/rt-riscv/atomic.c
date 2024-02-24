#include <stdbool.h>
#include <neorv32.h>

// Disable interrupts and return the previous mask.
static inline uint32_t disable(void)
{
    uint32_t prev_status = neorv32_cpu_csr_read(CSR_MSTATUS);
    uint32_t masked_status = prev_status & ~(1 << CSR_MSTATUS_MIE);
    neorv32_cpu_csr_write(CSR_MSTATUS, masked_status);
    return prev_status;
}

static inline void restore(uint32_t status)
{
    neorv32_cpu_csr_write(CSR_MSTATUS, status);
}

static inline void barrier_start(int memorder)
{
    if ((memorder == __ATOMIC_RELEASE) || (memorder == __ATOMIC_ACQ_REL) ||
        (memorder == __ATOMIC_SEQ_CST))
    {
        asm volatile("fence");
    }
}

static inline void barrier_end(int memorder)
{
    if ((memorder == __ATOMIC_CONSUME) || (memorder == __ATOMIC_ACQUIRE) ||
        (memorder == __ATOMIC_ACQ_REL) || (memorder == __ATOMIC_SEQ_CST))
    {
        asm volatile("fence");
    }
}

unsigned char __atomic_exchange_1(volatile void *ptr, unsigned char val,
                                  int memorder)
{
    volatile unsigned char *const p = ptr;

    const uint32_t mask = disable();
    barrier_start(memorder);
    const unsigned char old = *p;
    *p = val;
    barrier_end(memorder);
    restore(mask);

    return old;
}

unsigned __atomic_exchange_4(volatile void *ptr, unsigned val, int memorder)
{
    volatile unsigned *const p = ptr;

    const uint32_t mask = disable();
    barrier_start(memorder);
    const unsigned old = *p;
    *p = val;
    barrier_end(memorder);
    restore(mask);

    return old;
}

unsigned __atomic_fetch_add_4(volatile void *ptr, unsigned val, int memorder)
{
    volatile unsigned *const p = ptr;

    const uint32_t mask = disable();
    barrier_start(memorder);
    const unsigned old = *p;
    *p = old + val;
    barrier_end(memorder);
    restore(mask);

    return old;
}

unsigned __atomic_fetch_sub_4(volatile void *ptr, unsigned val, int memorder)
{
    volatile unsigned *const p = ptr;

    const uint32_t mask = disable();
    barrier_start(memorder);
    const unsigned old = *p;
    *p = old - val;
    barrier_end(memorder);
    restore(mask);

    return old;
}

unsigned __atomic_fetch_and_4(volatile void *ptr, unsigned val, int memorder)
{
    volatile unsigned *const p = ptr;

    const uint32_t mask = disable();
    barrier_start(memorder);
    const unsigned old = *p;
    *p = old & val;
    barrier_end(memorder);
    restore(mask);

    return old;
}

unsigned __atomic_fetch_or_4(volatile void *ptr, unsigned val, int memorder)
{
    volatile unsigned *const p = ptr;

    const uint32_t mask = disable();
    barrier_start(memorder);
    const unsigned old = *p;
    *p = old | val;
    barrier_end(memorder);
    restore(mask);

    return old;
}

bool __atomic_compare_exchange_1(volatile void *ptr, void *exp,
                                 unsigned char val, bool weak,
                                 int success_memorder, int fail_memorder)
{
    (void)weak;

    volatile unsigned char *const p = ptr;
    unsigned char *const e = exp;

    const uint32_t mask = disable();
    barrier_start(success_memorder);
    const unsigned char old = *p;
    const bool equal = old == *e;
    if (equal)
    {
        *p = val;
        barrier_end(success_memorder);
    }
    else
    {
        *e = old;
        barrier_end(fail_memorder);
    }
    restore(mask);
    return equal;
}

bool __atomic_compare_exchange_4(volatile void *ptr, void *exp, unsigned val,
                                 bool weak, int success_memorder,
                                 int fail_memorder)
{
    (void)weak;

    volatile uint32_t *const p = ptr;
    uint32_t *const e = exp;

    const uint32_t mask = disable();
    barrier_start(success_memorder);
    const uint32_t old = *p;
    const bool equal = old == *e;
    if (equal)
    {
        *p = val;
        barrier_end(success_memorder);
    }
    else
    {
        *e = old;
        barrier_end(fail_memorder);
    }
    restore(mask);
    return equal;
}
