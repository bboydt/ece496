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
        buffer <= {buffer[DEPTH-1:1], in};
        
        // only change out if the buffer contins all 1s or 0s
        if (buffer == (DEPTH)'d0) begin
            out <= 1'b0;
        end else if (buffer == {DEPTH{1'b1}}) begin
            out <= 1'b1;
        end;
    end

endmodule
