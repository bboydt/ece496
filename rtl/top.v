`include "ecp5_soc/utils/wishbone.v"
`include "ecp5_soc/package/wishbone_crossbar.v"
`include "ecp5_soc/cores/rom.v"
`include "ecp5_soc/cores/bram.v"


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
    output reg [2:0] user_leds_color,
    output reg [6:0] user_leds_en,
    inout reg [31:0] syzygy0_s
);
 
    reg sys_clk;
    reg sys_rst;

    always @(*) begin
        sys_clk = en;
        sys_rst = ~user_buttons[1];
    end



    // ROM0
    //

    `WISHBONE_REGS(rom0);
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

    `WISHBONE_REGS(ram0);
    bram #(
        .ADDR_WIDTH(32),
        .LENGTH(`RAM0_LENGTH)
    ) ram0 (
        .sys_clk(sys_clk),
        .sys_rst(sys_rst),
        `WISHBONE_PORT(wb, ram0)
    );


    // CPU0
    //

    reg [63:0] gpio;
    `WISHBONE_REGS(cpu0);
    neorv32_wrapper cpu0 (
        .sys_clk(sys_clk),
        .sys_rst_n(~sys_rst),
        .uart0_rx(syzygy0_s[0]),
        .uart0_tx(syzygy0_s[1]),
        .gpio_out(gpio),
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

    // Leds
    //

    reg en;
    reg [2:0] color;
    reg [32:0] counter;

    always @(*) begin
        user_leds_en = {en, rom0_ack, rom0_err, ram0_ack, ram0_err, 1'b0, gpio[0]};
        user_leds_color = 3'b110;
    end

    always @(posedge clk30) begin
        counter <= counter + 1;

        if (counter >= 1500000) begin
            counter <= 0;
            en <= ~en;
        end
    end
    
endmodule

