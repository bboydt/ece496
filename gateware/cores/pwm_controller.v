module pwm_controller #(
    parameter COUNT = 4
) (
    input sys_clk,
    input sys_rst,
    input pwm_clk,
    output [COUNT-1:0] pwms,
    `WISHBONE_SLAVE(wb)
);

    localparam ADDR_BITS = $clog2(COUNT);

    // Registers

    // | 31 - 16 | 15 -- 0 |
    // | Maximum | Compare |
    reg [15:0] vals [COUNT-1:0];
 
    // PWM Generation
    genvar i;
    generate
        for (i = 0; i < COUNT; i++) begin
            pwm #(
                .WIDTH(16)
            ) pwm (
                .clk(pwm_clk),
                .compare(vals[i][0*16+:16]),
                .out(pwms[i])
            );
        end
    endgenerate

    // Wishbone Interface
    always @(posedge sys_clk) begin
        if (wb_cyc & wb_stb) begin
            if (~wb_ack & ~wb_err) begin
                if (wb_we) begin
                    vals[wb_adr[2+:ADDR_BITS]] <= wb_mosi;
                    wb_ack <= 1'b1;
                    wb_err <= 1'b0;
                end else begin
                    wb_miso <= vals[wb_adr[2+:ADDR_BITS]][0+:16];
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
