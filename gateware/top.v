`include "ecp5_soc/utils/wishbone.v"
`include "ecp5_soc/package/wishbone_crossbar.v"
`include "ecp5_soc/cores/rom.v"
`include "ecp5_soc/cores/bram.v"

`include "pll.v"
`include "cores/leds.v"
`include "cores/vccio.v"

`define WISHBONE_CONNECT(MASTER_PREFIX, SLAVE_PREFIX) \
    SLAVE_PREFIX``_cyc <= MASTER_PREFIX``_cyc; \
    SLAVE_PREFIX``_stb <= MASTER_PREFIX``_stb; \
    SLAVE_PREFIX``_we <= MASTER_PREFIX``_we; \
    SLAVE_PREFIX``_tag <= MASTER_PREFIX``_tag; \
    SLAVE_PREFIX``_sel <= MASTER_PREFIX``_sel; \
    SLAVE_PREFIX``_adr <= MASTER_PREFIX``_adr; \
    SLAVE_PREFIX``_mosi <= MASTER_PREFIX``_mosi; \
    MASTER_PREFIX``_miso <= SLAVE_PREFIX``_miso; \
    MASTER_PREFIX``_ack <= SLAVE_PREFIX``_ack; \
    MASTER_PREFIX``_err <= SLAVE_PREFIX``_err

`define WISHBONE_ZERO_SLAVE(SLAVE_PREFIX) \
    SLAVE_PREFIX``_cyc  <= 0; \
    SLAVE_PREFIX``_stb  <= 0; \
    SLAVE_PREFIX``_we   <= 0; \
    SLAVE_PREFIX``_tag  <= 0; \
    SLAVE_PREFIX``_sel  <= 0; \
    SLAVE_PREFIX``_adr  <= 0; \
    SLAVE_PREFIX``_mosi <= 0

`define WISHBONE_ZERO_MASTER(MASTER_PREFIX) \
    MASTER_PREFIX``_miso <= 0; \
    MASTER_PREFIX``_ack  <= 0; \
    MASTER_PREFIX``_err  <= 0

module top (
    input clk30,
    input [1:0] user_buttons,
    output reset_n,
    output [2:0] user_leds_color,
    output [6:0] user_leds_en,
    output vccio_en,
    output [2:0] vccio_pdm,
    output spiflash_cs_n,
    output spiflash_mosi,
    input  spiflash_miso,
    inout reg [31:0] syzygy0_s
);
    reg sys_clk;
    reg sys_rst;

    always @(*) begin
        sys_rst = ~pll_locked | ~user_buttons[1];
    end

    assign reset_n = user_buttons[0];

    // PLL
    //

    wire pll_locked;
    ecp5_pll pll (
        .clk30(clk30),
        .clk90(sys_clk),
        .locked(pll_locked)
    );

    // SPIFLASH
    //

    wire spiflash_clk;
    USRMCLK usrmclk(.USRMCLKI(spiflash_clk), .USRMCLKTS(0));

    // ROM0
    //

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
    

    // RAM0
    //

    `WISHBONE_WIRES(ram0);
    bram #(
        .ADDR_WIDTH(32),
        .LENGTH(`RAM0_LENGTH)
    ) ram0 (
        .sys_clk(sys_clk),
        .sys_rst(sys_rst),
        `WISHBONE_PORT(wb, ram0)
    );


    // LED0
    //

    assign user_leds_color = 3'b100;
    assign user_leds_en = {gpio[0:6]};
//    reg pwm_clk;
//    reg [31:0] pwm_cnt;
//    always @(posedge clk30) begin
//        if (pwm_cnt > 50000) begin
//            pwm_clk <= ~pwm_clk;
//            pwm_cnt <= 0;
//        end else begin
//            pwm_cnt <= pwm_cnt + 1;
//        end
//    end
//
//    `WISHBONE_WIRES(led0);
//    led_controller led0 (
//        .sys_clk(sys_clk),
//        .sys_rst(sys_rst),
//        `WISHBONE_PORT(wb, led0),
//        .pwm_clk(pwm_clk),
//        .leds_color(user_leds_color),
//        .leds_en(user_leds_en)
//    );


    // VCCIO0
    //
    
    `WISHBONE_WIRES(vccio0);
    vccio_controller #(
        .CHANNEL_COUNT(3),
        .LEVEL_WIDTH(16)
    ) vccio0 (
        .sys_clk(sys_clk),
        .sys_rst(sys_rst),
        `WISHBONE_PORT(wb, vccio0),

        .clk30(clk30),
        .vccio_en(vccio_en),
        .vccio_pdm(vccio_pdm)
    );


    // Motor Pins
    //

    reg [31:0] mclk_counter;
    reg mclk;
    always @(posedge clk30) begin
        mclk_counter = mclk_counter + 1;
        if (mclk_counter > 30000) begin
            mclk = ~mclk;
            mclk_counter = 0;
        end else begin
            mclk = mclk;
            mclk_counter = mclk_counter;
        end
    end

    always @(*) begin
        syzygy0_s[12] = gpio[11] & mclk;
        syzygy0_s[14] = gpio[12];
        syzygy0_s[16] = gpio[13] & mclk;
        syzygy0_s[18] = gpio[14];
        syzygy0_s[20] = gpio[7 ] & mclk;
        syzygy0_s[22] = ~gpio[8 ];
        syzygy0_s[24] = gpio[9 ] & mclk;
        syzygy0_s[26] = ~gpio[10];
    end

    // CPU0
    //

    reg [63:0] gpio;
    `WISHBONE_WIRES(cpu0);
    neorv32_wrapper cpu0 (
        .sys_clk(sys_clk),
        .sys_rst_n(~sys_rst),
        .uart0_rx(syzygy0_s[0]),
        .uart0_tx(syzygy0_s[1]),
        .gpio_out(gpio),
        .xip_csn(spiflash_cs_n),
        .xip_clk(spiflash_clk),
        .xip_mosi(spiflash_mosi),
        .xip_miso(spiflash_miso),
        .jtag_trst(1'b1),
        .jtag_tck(syzygy0_s[15]),
        .jtag_tdi(syzygy0_s[17]),
        .jtag_tdo(syzygy0_s[19]),
        .jtag_tms(syzygy0_s[21]),
        `WISHBONE_PORT(bus, cpu0)
    );
    

    // Interconnect
    //
    
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

