`include "ecp5_soc/utils/wishbone.v"

module pdm #(
    // level width
    parameter LW = 16
) (
    input clk30,
    input [LW-1:0] level,
    output reg pdm
);
    // counter width
    localparam CW = LW + 2;

    reg [CW-1:0] counter;
    always @(posedge clk30) begin
        counter <= counter + {pdm, pdm, level};
        pdm <= counter[CW-1];
    end

endmodule

module vccio_controller #(
    parameter CHANNEL_COUNT = 3,
    parameter LEVEL_WIDTH = 16
) (
    input sys_clk,
    input sys_rst,
    `WISHBONE_SLAVE(wb),

    input clk30,
    output reg vccio_en,
    output [CHANNEL_COUNT-1:0] vccio_pdm
);

    localparam REG_COUNT = CHANNEL_COUNT + 1;
    localparam CTRL_REG_INDEX = 0; // use the first register as the control register
    localparam CHANNEL_REG_OFFS = 1;

    reg [31:0] registers[REG_COUNT-1:0];

    always @(*) begin
        vccio_en = 1;
    end

    genvar i;
    generate
    for (i = 0; i < CHANNEL_COUNT; i++) begin : channels
        always @(*) begin
            registers[i + CHANNEL_REG_OFFS] = 32'd6500;
        end
        pdm #(
            .LW(LEVEL_WIDTH)
        ) channel_pdm (
            .clk30(clk30),
            .level(registers[i + CHANNEL_REG_OFFS][LEVEL_WIDTH-1:0]),
            .pdm(vccio_pdm[i])
        );
    end
    endgenerate

endmodule
