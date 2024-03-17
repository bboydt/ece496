`include "ecp5_soc/utils/wishbone.v"
`include "ecp5_soc/package/wishbone_crossbar.v"
`include "ecp5_soc/cores/bram.v"
`include "ecp5_soc/cores/rom.v"

`define ENCS0_ADDR 32'hF0000000
`define ENCS0_MASK 32'h0000000F

`define LEDS0_ADDR 32'hF0000100
`define LEDS0_MASK 32'h000000FF

`define PWMS0_ADDR 32'hF0000200
`define PWMS0_MASK 32'hF00000FF

module soc (
    input sys_clk,
    input sys_rst,

    input vccio_clk,
    input led_clk,

    output [6:0] user_leds_en,
    output [2:0] user_leds_color,

    output vccio_en,
    output [2:0] vccio_pdm,

    output spiflash_cs_n,
    output spiflash_mosi,
    input spiflash_miso,

    input uart0_rx,
    output uart0_tx,

    inout [63:0] gpio,

    input [3:0] enc_a,
    input [3:0] enc_b,

    output [3:0] pwms,

    input jtag_tck,
    input jtag_tdi,
    output jtag_tdo,
    input jtag_tms
);
    // LEDs
    `WISHBONE_WIRES(leds0);
    leds leds0 (
        .sys_clk(sys_clk),
        .sys_rst(sys_rst),
        `WISHBONE_PORT(wb, leds0),

        .led_clk(led_clk),
        .leds(user_leds_en),
        .rgb_sel(user_leds_color)
    );

    // PWMs
    `WISHBONE_WIRES(pwms0);
    pwms #(
        .COUNT(4)
    ) pwms0 (
        .clk(sys_clk),
        .rst(sys_rst),
        `WISHBONE_PORT(wb, pwms0),
        .pwms(pwms)
    );

    // Processor
    `WISHBONE_WIRES(cpu0);
    neorv32_wrapper cpu0 (
        .sys_clk(sys_clk),
        .sys_rst_n(~sys_rst),

        .uart0_rx(uart0_rx),
        .uart0_tx(uart0_tx),

        .gpio_out(gpio),

        .xip_csn(spiflash_cs_n),
        .xip_clk(spiflash_clk),
        .xip_mosi(spiflash_mosi),
        .xip_miso(spiflash_miso),

        .jtag_trst(1'b1),
        .jtag_tck(jtag_tck),
        .jtag_tdi(jtag_tdi),
        .jtag_tdo(jtag_tdo),
        .jtag_tms(jtag_tms),

        `WISHBONE_PORT(bus, cpu0)
    );

    // SPIFlash
    wire spiflash_clk;
    USRMCLK usrmclk (
        .USRMCLKI(spiflash_clk),
        .USRMCLKTS(32'd0)
    );

    // VCCIO
    `WISHBONE_WIRES(vccio0);
    vccio_controller #(
        .CHANNEL_COUNT(3),
        .LEVEL_WIDTH(16)
    ) vccio0 (
        .sys_clk(sys_clk),
        .sys_rst(sys_rst),
        `WISHBONE_PORT(wb, vccio0),

        .clk30(vccio_clk),
        .vccio_en(vccio_en),
        .vccio_pdm(vccio_pdm)
    );

    // ROM and SRAM
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

    `WISHBONE_WIRES(ram0);
    bram #(
        .ADDR_WIDTH(32),
        .LENGTH(`RAM0_LENGTH)
    ) ram0 (
        .sys_clk(sys_clk),
        .sys_rst(sys_rst),
        `WISHBONE_PORT(wb, ram0)
    );

    // Encoders
    `WISHBONE_WIRES(encs0);
    encoders_core encs0 (
        .clk(sys_clk),
        .rst(sys_rst),
        .a(enc_a),
        .b(enc_b),
        `WISHBONE_PORT(wb, encs0)
    );

    // Wishbone Crossbar
    wishbone_crossbar #(
        .NM(1),
        .NS(5),
        .SA({`ROM0_ADDR, `RAM0_ADDR, `ENCS0_ADDR, `LEDS0_ADDR, `PWMS0_ADDR}),
        .SM({`ROM0_MASK, `RAM0_MASK, `ENCS0_MASK, `LEDS0_MASK, `PWMS0_MASK})
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

        .slaves_cyc ({rom0_cyc , ram0_cyc , encs0_cyc , leds0_cyc , pwms0_cyc }),
        .slaves_stb ({rom0_stb , ram0_stb , encs0_stb , leds0_stb , pwms0_stb }),
        .slaves_we  ({rom0_we  , ram0_we  , encs0_we  , leds0_we  , pwms0_we  }),
        .slaves_tag ({rom0_tag , ram0_tag , encs0_tag , leds0_tag , pwms0_tag }),
        .slaves_sel ({rom0_sel , ram0_sel , encs0_sel , leds0_sel , pwms0_sel }),
        .slaves_adr ({rom0_adr , ram0_adr , encs0_adr , leds0_adr , pwms0_adr }),
        .slaves_mosi({rom0_mosi, ram0_mosi, encs0_mosi, leds0_mosi, pwms0_mosi}),
        .slaves_miso({rom0_miso, ram0_miso, encs0_miso, leds0_miso, pwms0_miso}),
        .slaves_ack ({rom0_ack , ram0_ack , encs0_ack , leds0_ack , pwms0_ack }),
        .slaves_err ({rom0_err , ram0_err , encs0_err , leds0_err , pwms0_err }),
    );

endmodule
