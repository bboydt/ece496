module debounce #(
    parameter DEPTH = 32
) (
    input clk,
    input in,
    input reg out
);

    // Stores the DEPTH previous inputs
    reg [DEPTH-1:0] buffer;

    always @(posedge clk) begin
        // push into buffer
        buffer <= {buffer[1+:DEPTH-1], in};
        
        // only change out if the buffer contins all 1s or 0s
        if (buffer == {DEPTH{1'd1}}) begin
            out <= 1'b1;
        end else if (buffer == {DEPTH{1'd0}}) begin
            out <= 1'b0;
        end
    end

endmodule
