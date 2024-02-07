`include "ecp5_soc/utils/wishbone.v"
`include "ecp5_soc/cores/rom.v"
`include "ecp5_soc/cores/bram.v"

module top (
    input clk30,
    input [1:0] user_buttons,
    output reg [2:0] user_leds_color,
    output reg [6:0] user_leds_en
);
 
    reg sys_clk;
    reg sys_rst;

    always @(*) begin
        sys_clk = en;
        sys_rst = ~user_buttons[1];
    end



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
        


    reg en;
    reg [2:0] color;
    reg [32:0] counter;

    always @(*) begin
        user_leds_en = {en, {6{1'b0}}};
        user_leds_color = color;
    end

    always @(posedge clk30) begin
        counter <= counter + 1;

        if (counter >= 15000000) begin
            counter <= 0;
            en <= ~en;
            color <= color + 1;
        end
    end
    
endmodule

