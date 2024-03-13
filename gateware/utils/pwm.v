module pwm #(
    parameter WIDTH = 8,
) (
    input clk,
    input rst,
    
    input [WIDTH-1:0] max,
    input [WIDTH-1:0] threshold,

    output reg out
);
    reg [WIDTH:0] cnt;

    always @(posedge clk) begin
        cnt <= (cnt < max) ? (cnt + 1) : (0);
    end

    always @(*) begin
        out = (cnt < threshold);
    end
endmodule
