module pwm #(
    parameter WIDTH = 8,
) (
    input clk,
    input [WIDTH-1:0] compare,
    output reg out
);
    reg [WIDTH-1:0] counter;
    always @(posedge clk) begin
        counter <= (counter != {{WIDTH-1{1'b1}}, 1'b0}) ? (counter + 1) : ({WIDTH{1'b0}});
        out <= (counter < compare);
    end
endmodule
