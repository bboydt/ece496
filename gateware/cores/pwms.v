// ngl, this isn't the best thing I've ever written.
// In fact, it's probably one of the worst. 
// Clock division and enable don't work.
// And I'm not going to fix it because lazy. :(
//
// whomp whomp, i'm rewriting it

module new_pwm #(
    parameter WIDTH = 8
) (
    input clk,
    input [WIDTH-1:0] cmp,
    output reg pwm
);
    reg [WIDTH-1:0] cnt;

    always @(*) begin
        pwm = cnt < cmp;
    end

    always @(posedge clk) begin
        cnt <= cnt + 1;
    end
endmodule

module pwms #(
    parameter COUNT = 4
) (
    input clk,
    input rst,

    input pwm_clk,
    output [COUNT-1:0] pwms,

    `WISHBONE_SLAVE(wb)
);

    localparam COUNT_BITS = $clog2(COUNT);

    // Registers

    // | 31 - 16 | 15 -- 0 |
    // | Maximum | Compare |
    reg [31:0] vals [COUNT-1:0];
 
    // PWM Generation
    genvar i;
    generate
        for (i = 0; i < COUNT; i++) begin
            pwm #(
                .WIDTH(8)
            ) pwm (
                .clk(pwm_clk),
                .max(8'hFF),
                .threshold(vals[i][0*8+:8]),
                .out(pwms[i])
            );
        end
    endgenerate

    // Wishbone Interface
    always @(posedge clk) begin
        if (wb_cyc & wb_stb) begin
            if (~wb_ack & ~wb_err) begin
                if (wb_we) begin
                    vals[wb_adr[2+:COUNT_BITS]] <= wb_mosi;
                    wb_ack <= 1'b1;
                    wb_err <= 1'b0;
                end else begin
                    wb_miso <= vals[wb_adr[2+:COUNT_BITS]];
                    wb_ack <= 1'b1;
                    wb_err <= 1'b0;
                end
            end
        end else begin
            wb_ack <= 1'b0;
            wb_err <= 1'b0;
        end
    end

endmodule
