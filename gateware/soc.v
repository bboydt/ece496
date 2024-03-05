`include "ecp5_soc/utils/wishbone.v"
`include "ecp5_soc/package/wishbone_crossbar.v"
`include "ecp5_soc/cores/bram.v"
`include "ecp5_soc/cores/rom.v"

module soc (
    input sys_clk,
    input sys_rst,

    input vccio_clk,

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

    input jtag_tck,
    input jtag_tdi,
    output jtag_tdo,
    input jtag_tms
);
    // LEDs
    assign user_leds_color = 3'b110;
    assign user_leds_en = {gpio[0:6]};

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

    // Wishbone Crossbar
    wishbone_crossbar #(
        .NM(1),
        .NS(2),
        .SA({`ROM0_ADDR, `RAM0_ADDR}),
        .SM({`ROM0_MASK, `ROM0_MASK})
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

        .slaves_cyc ({rom0_cyc , ram0_cyc }),
        .slaves_stb ({rom0_stb , ram0_stb }),
        .slaves_we  ({rom0_we  , ram0_we  }),
        .slaves_tag ({rom0_tag , ram0_tag }),
        .slaves_sel ({rom0_sel , ram0_sel }),
        .slaves_adr ({rom0_adr , ram0_adr }),
        .slaves_mosi({rom0_mosi, ram0_mosi}),
        .slaves_miso({rom0_miso, ram0_miso}),
        .slaves_ack ({rom0_ack , ram0_ack }),
        .slaves_err ({rom0_err , ram0_err }),
    );

endmodule
