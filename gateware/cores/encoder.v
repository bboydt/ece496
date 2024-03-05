module encoder (
    input clk,
    input rst,

    input a,
    input b,

    output reg [31:0] position
);

    reg [31:0] tmp_position;
    always @(posedge a) begin
        tmp_position <= (b) ? tmp_position + 1 : tmp_position - 1;
    end

    always @(posedge clk) begin
        position <= (~rst) ? tmp_position : 32'd0;
    end

endmodule
