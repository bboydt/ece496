module crg (
    input clk_in,
    input rst_in,

    output sys_clk,
    output sys_rst,
);

    wire pll_locked;
    assign sys_rst = ~pll_locked | rst_in;

    ecp5_pll pll0 (
        .clk30(clk_in),
        .clk90(sys_clk),
        .locked(pll_locked)
    );

endmodule
