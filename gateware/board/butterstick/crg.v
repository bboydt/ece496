module crg (
    input clk_in,
    input rst_in,

    output sys_clk,
    output sys_rst,

    output led_clk,
    output pwm_clk,
);

    wire pll_locked;
    assign sys_rst = ~pll_locked | rst_in;

    ecp5_pll pll0 (
        .clk30(clk_in),
        .clk90(sys_clk),
        .clk1(pwm_clk),
        .clk250k(led_clk),
        .locked(pll_locked)
    );

endmodule
