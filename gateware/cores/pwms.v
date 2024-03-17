// ngl, this isn't the best thing I've ever written.
// In fact, it's probably one of the worst. 
// Clock division and enable don't work.
// And I'm not going to fix it because lazy. :(

module pwms #(
    parameter COUNT = 4
) (
    input clk,
    input rst,

    `WISHBONE_SLAVE(wb),

    output [COUNT-1:0] pwms
);

    localparam COUNT_BITS = $clog2(COUNT);

    // Registers

    // |  4 | 3 -- 0 |
    // | EN | CLKDIV |
    reg [4:0] ctrls [COUNT-1:0];
    // | 31 - 16 | 15 -- 0 |
    // | Maximum | Compare |
    reg [31:0] vals [COUNT-1:0];
 
    // Clock Division
    reg [2**4-1:0] clkdiv;
    always @(posedge clk) begin
        clkdiv <= clkdiv + 1;
    end

    // PWM Generation
    genvar i;
    generate
        for (i = 0; i < COUNT; i++) begin
            wire [3:0] clkdiv_sel;
            assign clkdiv_sel = ctrls[i][3:0];
            
            wire en;
            assign en = ctrls[i][4];

            wire [15:0] maxval;
            assign maxval = vals[i][31:16];

            wire [15:0] cmpval;
            assign cmpval = vals[i][15:0];

            wire out;
            assign pwms[i] = out;

            pwm #(
                .WIDTH(16)
            ) pwm (
                .clk(clkdiv[3]),
                .rst(rst),
                .max(maxval),
                .threshold(cmpval),
                .out(out)
            );
        end
    endgenerate

    // Wishbone Interface
    always @(posedge clk) begin
        if (wb_cyc & wb_stb) begin
            if (~wb_ack & ~wb_err) begin
                if (wb_we) begin
                    if (~wb_adr[COUNT_BITS]) begin
                        if (wb_sel[0])
                            ctrls[wb_adr[COUNT_BITS-1:0]] <= wb_mosi[4:0];
                    end else begin
                        if (wb_sel[3])
                            vals[wb_adr[COUNT_BITS-1:0]][0*8+:8] <= wb_mosi[3*8+:8];
                        if (wb_sel[2])
                            vals[wb_adr[COUNT_BITS-1:0]][1*8+:8] <= wb_mosi[2*8+:8];
                        if (wb_sel[1])
                            vals[wb_adr[COUNT_BITS-1:0]][2*8+:8] <= wb_mosi[1*8+:8];
                        if (wb_sel[0])
                            vals[wb_adr[COUNT_BITS-1:0]][3*8+:8] <= wb_mosi[0*8+:8];
                    end
                    wb_ack <= 1'b1;
                    wb_err <= 1'b0;
                end else begin
                    if (~wb_adr[COUNT_BITS]) begin
                        wb_miso <= { (32-COUNT_BITS)'d0, ctrls[wb_adr[COUNT_BITS-1:0]] };
                    end else begin
                        wb_miso <= vals[wb_adr[COUNT_BITS-1:0]];
                    end
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
