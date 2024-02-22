#include <rt/context.h>
#include <rt/task.h>
#include <rt/tick.h>
#include <rt/trap.h>

#include <stdint.h>
#include <neorv32.h>

#include "common.h"

struct context
{
    uint32_t ra;
    uint32_t __pad0;
    uint32_t gp;
    uint32_t tp;
    uint32_t t0;
    uint32_t t1;
    uint32_t t2;
    uint32_t s0;
    uint32_t s1;
    uint32_t a0;
    uint32_t a1;
    uint32_t a2;
    uint32_t a3;
    uint32_t a4;
    uint32_t a5;
    uint32_t a6;
    uint32_t a7;
    uint32_t s2;
    uint32_t s3;
    uint32_t s4;
    uint32_t s5;
    uint32_t s6;
    uint32_t s7;
    uint32_t s8;
    uint32_t s9;
    uint32_t s10;
    uint32_t s11;
    uint32_t t3;
    uint32_t t4;
    uint32_t t5;
    uint32_t t6;
    uint32_t __pad1;
};

void *context_init(void *stack, size_t stack_size)
{
    void *const sp = (char*)stack + stack_size;
    struct context *ctx = sp;
    --ctx;
    for (int i = 0; i < sizeof(struct context); i++)
    {
        ((uint8_t*)ctx)[i] = 0;
    }
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

