module motor (
    input sys_clk,
    input sys_rst,
    input pwm_clk,

    input [1:0] enc,
    output reg pwm,
    input reg en,
    output reg [31:0] pos,
    input reg [7:0] pwm_cmp,

);
    // encoder
    reg a;
    reg b;
    reg prev_a;
    reg [31:0] tmp_pos;
    reg [47:0] a_samples;
    reg [47:0] b_samples;

    always @(posedge sys_clk) begin
        // rising edge
        if ((a ^ prev_a) & a) begin
            tmp_pos <= (b) ? (tmp_pos + 1) : (tmp_pos - 1);
        end
        prev_a <= a;
        pos <= (~sys_rst) ? tmp_pos : 32'd0;

        a_samples <= {a_samples[46:0], enc[0]};
        b_samples <= {b_samples[46:0], enc[1]};

        if (a_samples == 48'h0) begin
            a <= 1'b0;
        end else if (a_samples == 48'hFFFFFFFFFF) begin
            a <= 1'b1;
        end

        if (b_samples == 48'h0) begin
            b <= 1'b0;
        end else if (b_samples == 48'hFFFFFFFFFF) begin
            b <= 1'b1;
        end


    end

    // pwm
    reg [7:0] pwm_counter;
    always @(posedge pwm_clk) begin
        pwm_counter <= (pwm_counter == 8'hFE) ? (8'h0) : (pwm_counter + 1);
        pwm <= en & (pwm_counter < pwm_cmp);
    end
endmodule

module motor_controller #(
    parameter COUNT = 4
) (
    input sys_clk,
    input sys_rst,

    input pwm_clk,

    input [(2*COUNT)-1:0] enc,
    output [COUNT-1:0] pwm,
    output [COUNT-1:0] dir,

    `WISHBONE_SLAVE(wb)
);

    wire [31:0] positions[COUNT-1:0];
    reg [COUNT-1:0] motor_en;
    reg [COUNT-1:0] motor_dir;
    reg [8:0] pwm_cmps[COUNT-1:0];

    localparam ADDR_BITS = $clog2(COUNT);

    assign dir = motor_dir;

    genvar i;
    generate
    for (i = 0; i < COUNT; i++) begin
        motor motor_controller (
            .sys_clk(sys_clk),
            .sys_rst(sys_rst),
            .pwm_clk(pwm_clk),
            .enc(enc[2*i+:2]),
            .pwm(pwm[i]),
            .en(motor_en[i]),
            .pos(positions[i]),
            .pwm_cmp(pwm_cmps[i])
        );
    end
    endgenerate

    always @(posedge sys_clk) begin
        if (wb_cyc & wb_stb) begin
            if (~wb_ack & ~wb_err) begin
                casex (wb_adr[7:0])
                    // control registers cycles
                    8'b0xxxx: begin
                        if (wb_we) begin
                            motor_en[wb_adr[2+:ADDR_BITS]] <= (wb_sel[3]) ? wb_mosi[31] : 1'b0;
                            motor_dir[wb_adr[2+:ADDR_BITS]] <= (wb_sel[3]) ? wb_mosi[30] : 1'b0;
                            pwm_cmps[wb_adr[2+:ADDR_BITS]] <= (wb_sel[0]) ? wb_mosi[0*8+:8] : 8'b0;
                            wb_ack <= 1'b1;
                            wb_err <= 1'b0;
                        end else begin
                            wb_miso <= {
                                motor_en[wb_adr[2+:ADDR_BITS]], 
                                motor_dir[wb_adr[2+:ADDR_BITS]], 
                                22'd0, 
                                pwm_cmps[wb_adr[2+:ADDR_BITS]]
                            };
                            wb_ack <= 1'b1;
                            wb_err <= 1'b0;
                        end
                    end
                   
                    // encoder positions
                    8'b1xxxx: begin
                        if (wb_we) begin
                            wb_ack <= 1'b0;
                            wb_err <= 1'b1;
                        end else begin
                            wb_miso <= positions[wb_adr[2+:ADDR_BITS]];
                            wb_ack <= 1'b1;
                            wb_err <= 1'b0;
                        end
                    end
                endcase
            end
        end else begin
            wb_ack <= 1'b0;
            wb_err <= 1'b0;
        end
    end

endmodule
