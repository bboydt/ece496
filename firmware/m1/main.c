#include <neorv32.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include "ascii.h"

// Printing
//

#define CTRL_UART  NEORV32_UART0
#define DEBUG_UART NEORV32_UART0

#define CTRL_UART_BAUD 115200
#define DEBUG_UART_BAUD 115200

#define stdin CTRL_UART
#define stdout DEBUG_UART
#define stderr DEBUG_UART

#define fprintf(uart, fmt, ...) neorv32_uart_printf(uart, fmt, ## __VA_ARGS__)
#define printf(fmt, ...) fprintf(stdout, fmt, ## __VA_ARGS__)

#define fputs(uart, str) neorv32_uart_puts(uart, str)

#define print_error(fmt, ...) fprintf(stderr, ASCII_RED "[error]" ASCII_RESET " " fmt "\n", ## __VA_ARGS__) 
#define print_warn(fmt, ...)  fprintf(stderr, ASCII_YELLOW "[warn]" ASCII_RESET "  " fmt "\n", ## __VA_ARGS__) 
#define print_log(fmt, ...)   fprintf(stderr, "[info]  " fmt "\n", ## __VA_ARGS__)

#define getc(uart) neorv32_uart_getc(uart)

// Motor Control
//

// This is how the car is modeled.
// The [\\] and [//] are top down views of the wheels. 
// The slashes match the rollers.
//
//          \ /
//         LIDAR
//     [\\]=====[//]
//      || 3   0 ||
//      ||  CAR  ||
//      || 2   1 ||
//     [//]=====[\\]
// Y     ||%BOX%||
// ^     ||%BOX%||
// |
// + -- > X


#define ASCII_UP    'w'
#define ASCII_DOWN  's'
#define ASCII_RIGHT 'd'
#define ASCII_LEFT  'a'

static uint32_t input = 0;

void uart0_rx_handler()
{
    neorv32_gpio_pin_set(1);
    // store input
    char c = getc(stdin);
    switch (c)
    {
        case ASCII_UP:
            input |= 0b1000;
            break;
        case ASCII_DOWN: 
            input |= 0b0100;
            break;
        case ASCII_RIGHT:
            input |= 0b0010;
            break;
        case ASCII_LEFT:
            input |= 0b0001;
            break;
        default: 
            neorv32_uart_putc(DEBUG_UART, c);
            break;
    }
    
    // bit we clear depends on which uart we're using
    neorv32_cpu_csr_clr(CSR_MIP, (1 << 2) << 16); 
}

static inline void uart_init(void)
{
    neorv32_uart_setup(CTRL_UART,  CTRL_UART_BAUD, ~1);
    //@todo reenable this if CTRL_UART != DEBUG_UART
    //neorv32_uart_setup(DEBUG_UART, DEBUG_UART_BAUD, ~0);
    
    // bit we clear depends on which uart we're using
    neorv32_cpu_csr_set(CSR_MIE, (1 << 2) << 16);
}


#define FRONT_RIGHT 0
#define BACK_RIGHT  1
#define BACK_LEFT   2
#define FRONT_LEFT  3

#define MOTOR_GPIO_OFFS 7
#define MOTOR(n, en, dir) ((((dir) << 1) | (en)) << (2*(n)))

static const uint32_t motor_states[9] =
{
    // (XX, YY)
    
    // 0
    // ( 0,  0)
    MOTOR(FRONT_RIGHT, 0, 0) |
    MOTOR(BACK_RIGHT , 0, 0) |
    MOTOR(BACK_LEFT  , 0, 0) |
    MOTOR(FRONT_LEFT , 0, 0),
    
    // 1
    // ( 0, +1)
    MOTOR(FRONT_RIGHT, 1, 1) |
    MOTOR(BACK_RIGHT , 1, 1) |
    MOTOR(BACK_LEFT  , 1, 1) |
    MOTOR(FRONT_LEFT , 1, 1),
    
    // 2
    // (+1, +1)
    MOTOR(FRONT_RIGHT, 0, 0) |
    MOTOR(BACK_RIGHT , 1, 1) |
    MOTOR(BACK_LEFT  , 0, 0) |
    MOTOR(FRONT_LEFT , 1, 1),
    
    // 3
    // (+1,  0)
    MOTOR(FRONT_RIGHT, 1, 0) |
    MOTOR(BACK_RIGHT , 1, 1) |
    MOTOR(BACK_LEFT  , 1, 0) |
    MOTOR(FRONT_LEFT , 1, 1),
    
    // 4
    // (+1, -1)
    MOTOR(FRONT_RIGHT, 1, 0) |
    MOTOR(BACK_RIGHT , 0, 0) |
    MOTOR(BACK_LEFT  , 1, 0) |
    MOTOR(FRONT_LEFT , 0, 0),
    
    // 5
    // ( 0, -1)
    MOTOR(FRONT_RIGHT, 1, 0) |
    MOTOR(BACK_RIGHT , 1, 0) |
    MOTOR(BACK_LEFT  , 1, 0) |
    MOTOR(FRONT_LEFT , 1, 0),
    
    // 6
    // (-1, -1)
    MOTOR(FRONT_RIGHT, 0, 0) |
    MOTOR(BACK_RIGHT , 1, 0) |
    MOTOR(BACK_LEFT  , 0, 0) |
    MOTOR(FRONT_LEFT , 1, 0),
    
    // 7
    // (-1,  0)
    MOTOR(FRONT_RIGHT, 1, 1) |
    MOTOR(BACK_RIGHT , 1, 0) |
    MOTOR(BACK_LEFT  , 1, 1) |
    MOTOR(FRONT_LEFT , 1, 0),

    // 8
    // (-1, +1)
    MOTOR(FRONT_RIGHT, 1, 1) |
    MOTOR(BACK_RIGHT , 0, 0) |
    MOTOR(BACK_LEFT  , 1, 1) |
    MOTOR(FRONT_LEFT , 0, 0),
};


static const uint32_t decoded_inputs[16] =
{
    0, 7, 3, 0, 5, 6, 4, 5, 1, 8, 2, 1, 0, 7, 3, 0
};

void mti_handler()
{
    // decode input
    uint32_t decoded_input = decoded_inputs[input];
    neorv32_gpio_pin_clr(1);

    //neorv32_uart_puts(DEBUG_UART, motor_state_strs[decoded_input]);

    // update motor state
    uint32_t motor_state = motor_states[decoded_input];
    uint32_t prev_gpio_state = NEORV32_GPIO->OUTPUT_LO & ~(0x7FBC);
    NEORV32_GPIO->OUTPUT_LO = prev_gpio_state | (motor_state << MOTOR_GPIO_OFFS) | (input << 2);
    input = 0;

    // blink led
    static uint32_t tick_count;
    if (tick_count++ % 10 == 0)
        neorv32_gpio_pin_toggle(0);
    
    // update timecmp
    neorv32_mtime_set_timecmp(neorv32_mtime_get_timecmp() + (NEORV32_SYSINFO->CLK / 10));
}

static inline void mtime_init(void)
{
    // set mtime to fire in 1ms
    neorv32_mtime_set_timecmp(neorv32_mtime_get_time() + (NEORV32_SYSINFO->CLK / 10));
    neorv32_cpu_csr_set(CSR_MIE, 1 << CSR_MIE_MTIE);
}

static inline void gpio_init(void)
{
    NEORV32_GPIO->OUTPUT_LO = 0;
}


// Entry Point
//

int main(void)
{
    mtime_init();
    uart_init();
    gpio_init();

    // enabled interrupts
    neorv32_cpu_csr_set(CSR_MSTATUS, 1 << CSR_MSTATUS_MIE);

    // enable advertising
    print_error("This is an error.");
    print_warn("This is a warning.");
    print_log("This is a log.");

    for (;;);
}
