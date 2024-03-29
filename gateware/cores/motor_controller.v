module motor_controller #(

) (
    input sys_clk,
    input sys_rst,

    input enc_a,
    input enc_b,

    input pwm_clk,
    output reg pwm,
);

    // pwm
    reg en;
    reg [7:0] pwm_counter;
    reg [7:0] pwm_compare;
    always @(posedge pwm_clk) begin
        pwm_counter <= (pwm_counter == 8'hFE) ? 8'h0 : pwm_counter + 1;
        pwm <= en & (pwm_counter < pwm_compare);
    end


endmodule
