module top (
    input clk30,
    output reset_n,

    input [1:0] user_buttons,

    output [6:0] user_leds_en,
    output [2:0] user_leds_color,

    output vccio_en,
    output [2:0] vccio_pdm,

    output spiflash_cs_n,
    output spiflash_mosi,
    input spiflash_miso,

    inout [31:0] syzygy0
);

    // Power and Clock Domain
    wire sys_rst;
    wire sys_clk;
    wire led_clk;
    wire pwm_clk;

    reg [10:0] clk_divider;
    always @(posedge clk30) begin
        clk_divider <= clk_divider + 1;
    end
    assign led_clk = clk_divider[7];
    assign pwm_clk = clk_divider[0];

    assign reset_n = user_buttons[0];

    crg crg0 (
        .clk_in(clk30),
        .rst_in(~user_buttons[0]),

        .sys_clk(sys_clk),
        .sys_rst(sys_rst)
    );

    // Spiflash

    wire [63:0] gpio_out;

    assign syzygy0[19] = ~gpio_out[3];
    assign syzygy0[17] = ~gpio_out[2];
    assign syzygy0[15] = gpio_out[1];
    assign syzygy0[13] = gpio_out[0];

    // SoC
    soc soc0 (
        .sys_clk(sys_clk),
        .sys_rst(sys_rst),
        .vccio_clk(clk30),
        .led_clk(led_clk),
        .pwm_clk(pwm_clk),
        .user_leds_en(user_leds_en),
        .user_leds_color(user_leds_color),
        .vccio_en(vccio_en),
        .vccio_pdm(vccio_pdm),
        .spiflash_cs_n(spiflash_cs_n),
        .spiflash_mosi(spiflash_mosi),
        .spiflash_miso(spiflash_miso),
        .uart0_rx(syzygy0[1]),
        .uart0_tx(syzygy0[3]),
        .uart1_rx(syzygy0[5]),
        .uart1_tx(syzygy0[7]),
        .enc_a({syzygy0[26], syzygy0[24], syzygy0[22], syzygy0[20]}),
        .enc_b({syzygy0[27], syzygy0[25], syzygy0[23], syzygy0[21]}),
        .pwms({syzygy0[0], syzygy0[18], syzygy0[16], syzygy0[14], syzygy0[12]}),
        .jtag_tck(1'b0),//syzygy0[2]),
        .jtag_tdi(1'b0),//syzygy0[4]),
        //.jtag_tdo(syzygy0[6]),
        .jtag_tms(1'b0),//syzygy0[8])
        .gpio_out(gpio_out)
    );

endmodule
