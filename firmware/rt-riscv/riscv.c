#include <rt/context.h>
#include <rt/task.h>
#include <rt/tick.h>
#include <rt/trap.h>

#include <stdint.h>
#include <neorv32.h>

#include "common.h"

struct context
{
    uint32_t ra, sp, gp, tp;
    uint32_t t0, t1, t2, t3, t4, t5, t6;
    uint32_t a0, a1, a2, a3, a4, a5, a6, a7;
    uint32_t s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11;
};

void *context_init(void *stack, size_t stack_size)
{
    void *const sp = (char*)stack + stack_size;
    struct context *ctx = sp;
    --ctx;
    return ctx;
}

__attribute__((noreturn)) void rt_task_entry(void (*fn)(void));

__attribute__((noreturn)) void rt_task_entry_arg(uintptr_t arg, void (*fn)(uintptr_t));

void *rt_context_init(void (*fn)(void), void *stack, size_t stack_size)
{
    struct context *const ctx = context_init(stack, stack_size);
    ctx->ra = (uint32_t)rt_task_entry;
    ctx->a0 = (uint32_t)fn;
    return ctx;
}

void *rt_context_init_arg(void (*fn)(uintptr_t), uintptr_t arg, void *stack, size_t stack_size)
{
    struct context *const ctx = context_init(stack, stack_size);
    ctx->ra = (uint32_t)rt_task_entry_arg;
    ctx->a0 = (uint32_t)arg;
    ctx->a1 = (uint32_t)fn;
    return ctx;
}

__attribute__((noreturn, weak)) void rt_idle(void)
{
    for (;;)
    {
        __asm__("wfi");
    }
}

__attribute__((noreturn, weak)) void rt_trap(void)
{
    for (;;)
    {
        __asm__("ebreak");
    }
}

void rt_syscall(void)
{
    __asm__("ecall");
}

// @todo figure out what to actually do here
__attribute__((alias("rt_syscall"))) void rt_syscall_pend(void);

void mti_handler(void)
{
    neorv32_mtime_set_timecmp(neorv32_mtime_get_timecmp() + (NEORV32_SYSINFO->CLK / 1000));
    rt_tick_advance();
}

