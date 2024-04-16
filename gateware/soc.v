`include "ecp5_soc/utils/wishbone.v"
`include "ecp5_soc/package/wishbone_crossbar.v"
`include "ecp5_soc/cores/bram.v"
`include "ecp5_soc/cores/rom.v"

`define LEDS0_ADDR   32'hF0000000
`define LEDS0_MASK   32'h000000FF

`define MOTORS0_ADDR 32'hF0000100
`define MOTORS0_MASK 32'h000000FF

`define SERVOS0_ADDR 32'hF0000200
`define SERVOS0_MASK 32'h000000FF

module soc (

    // Clock / Reset
    input sys_clk,
    input sys_rst,
    input vccio_clk,
    input led_clk,
    input motor_pwm_clk,
    input servo_pwm_clk,

    // SPIFlash
    output spiflash_cs_n,
    output spiflash_mosi,
    input spiflash_miso,

    // LEDs
    output [6:0] user_leds_en,
    output [2:0] user_leds_color,

    // Motors
    input [7:0] motor_encs,
    output [3:0] motor_pwms,
    output [3:0] motor_dirs,

    // Servos
    output servo_pwms,

    // VCCIO
    output vccio_en,
    output [2:0] vccio_pdm,

    // UART
    input uart0_rx,
    output uart0_tx,
    input uart1_rx,
    output uart1_tx,

    // I2C
    inout reg i2c_scl,
    inout reg i2c_sda,

    // GPIO
    inout [63:0] gpio_out,

    // JTAG
    input jtag_tck,
    input jtag_tdi,
    output jtag_tdo,
    input jtag_tms
);
    // I2C
    wire i2c_scl_i;
    wire i2c_sda_i;
    wire i2c_scl_o;
    wire i2c_sda_o;
    BB i2c_scl_buffer (.T(i2c_scl_o), .I(i2c_scl_o), .O(i2c_scl_i), .B(i2c_scl));
    BB i2c_sda_buffer (.T(i2c_sda_o), .I(i2c_sda_o), .O(i2c_sda_i), .B(i2c_sda));

    // Processor
    `WISHBONE_WIRES(cpu0);
    neorv32_wrapper cpu0 (
        .sys_clk(sys_clk),
        .sys_rst_n(~sys_rst),

        .uart0_rx(uart0_rx),
        .uart0_tx(uart0_tx),
        .uart1_rx(uart1_rx),
        .uart1_tx(uart1_tx),

        .twi_scl_i(i2c_scl_i),
        .twi_sda_i(i2c_sda_i),
        .twi_scl_o(i2c_scl_o),
        .twi_sda_o(i2c_sda_o),

        .gpio_out(gpio_out),

        .xip_csn(spiflash_cs_n),
        .xip_clk(spiflash_clk),
        .xip_mosi(spiflash_mosi),
        .xip_miso(spiflash_miso),

        //.jtag_trst(1'b1),
        //.jtag_tck(jtag_tck),
        //.jtag_tdi(jtag_tdi),
        //.jtag_tdo(jtag_tdo),
        //.jtag_tms(jtag_tms),

        `WISHBONE_PORT(bus, cpu0)
    );

    // ROM
    `WISHBONE_WIRES(rom0);
    rom #(
        .ADDR_WIDTH(32),
        .LENGTH(`ROM0_LENGTH),
        .INIT_FILE(`ROM0_INIT_FILE)
    ) rom0 (
        .sys_clk(sys_clk),
        .sys_rst(sys_rst),
        `WISHBONE_PORT(wb, rom0)
    );

    // RAM
    `WISHBONE_WIRES(ram0);
    bram #(
        .ADDR_WIDTH(32),
        .LENGTH(`RAM0_LENGTH)
    ) ram0 (
        .sys_clk(sys_clk),
        .sys_rst(sys_rst),
        `WISHBONE_PORT(wb, ram0)
    );

    // SPIFlash
    wire spiflash_clk;
    USRMCLK usrmclk (
        .USRMCLKI(spiflash_clk),
        .USRMCLKTS(32'd0)
    );

    // LEDs
    `WISHBONE_WIRES(leds0);
    led_controller leds0 (
        .sys_clk(sys_clk),
        .sys_rst(sys_rst),
        `WISHBONE_PORT(wb, leds0),

        .led_clk(led_clk),
        .leds(user_leds_en),
        .rgb_sel(user_leds_color)
    );

    // Motors
    `WISHBONE_WIRES(motors0);
    motor_controller #(.COUNT(4)) motors0 (
        .sys_clk(sys_clk),
        .sys_rst(sys_rst),
        .pwm_clk(motor_pwm_clk),
        .enc(motor_encs),
        .pwm(motor_pwms),
        .dir(motor_dirs),
        `WISHBONE_PORT(wb, motors0)
    );

    // Servos
    `WISHBONE_WIRES(servos0);
    pwm_controller servos0 (
        .sys_clk(sys_clk),
        .sys_rst(sys_rst),
        .pwm_clk(servo_pwm_clk),
        .pwm(servo_pwms),
        `WISHBONE_PORT(wb, servos0)
    );

    // VCCIO
    `WISHBONE_WIRES(vccio0);
    vccio_controller #(
        .CHANNEL_COUNT(3),
        .LEVEL_WIDTH(16)
    ) vccio0 (
        .sys_clk(sys_clk),
        .sys_rst(sys_rst),
        .clk30(vccio_clk),
        .vccio_en(vccio_en),
        .vccio_pdm(vccio_pdm),
        `WISHBONE_PORT(wb, vccio0)
    );

    // Wishbone Crossbar
    wishbone_crossbar #(
        .NM(1),
        .NS(5),
        .SA({`ROM0_ADDR, `RAM0_ADDR, `LEDS0_ADDR, `MOTORS0_ADDR, `SERVOS0_ADDR}),
        .SM({`ROM0_MASK, `RAM0_MASK, `LEDS0_MASK, `MOTORS0_MASK, `SERVOS0_MASK})
    ) wbx0 (
        .sys_clk(sys_clk),
        .sys_rst(sys_rst),

        .masters_cyc ({cpu0_cyc }),
        .masters_stb ({cpu0_stb }),
        .masters_we  ({cpu0_we  }),
        .masters_tag ({cpu0_tag }),
        .masters_sel ({cpu0_sel }),
        .masters_adr ({cpu0_adr }),
        .masters_mosi({cpu0_mosi}),
        .masters_miso({cpu0_miso}),
        .masters_ack ({cpu0_ack }),
        .masters_err ({cpu0_err }),

        .slaves_cyc ({rom0_cyc , ram0_cyc , leds0_cyc , motors0_cyc , servos0_cyc }),
        .slaves_stb ({rom0_stb , ram0_stb , leds0_stb , motors0_stb , servos0_stb }),
        .slaves_we  ({rom0_we  , ram0_we  , leds0_we  , motors0_we  , servos0_we  }),
        .slaves_tag ({rom0_tag , ram0_tag , leds0_tag , motors0_tag , servos0_tag }),
        .slaves_sel ({rom0_sel , ram0_sel , leds0_sel , motors0_sel , servos0_sel }),
        .slaves_adr ({rom0_adr , ram0_adr , leds0_adr , motors0_adr , servos0_adr }),
        .slaves_mosi({rom0_mosi, ram0_mosi, leds0_mosi, motors0_mosi, servos0_mosi}),
        .slaves_miso({rom0_miso, ram0_miso, leds0_miso, motors0_miso, servos0_miso}),
        .slaves_ack ({rom0_ack , ram0_ack , leds0_ack , motors0_ack , servos0_ack }),
        .slaves_err ({rom0_err , ram0_err , leds0_err , motors0_err , servos0_err }),
    );

endmodule
