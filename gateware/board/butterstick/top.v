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
    wire sys_clk;
    wire sys_rst;

    assign reset_n = user_buttons[0];

    crg crg0 (
        .clk_in(clk30),
        .rst_in(~user_buttons[0]),

        .sys_clk(sys_clk),
        .sys_rst(sys_rst),
    );

    // Spiflash

    // Slow Clock (1kHz)    
    reg slow_clk;
    reg [31:0] slow_clk_counter;
    always @(posedge clk30) begin
        if (slow_clk_counter >= 300) begin
            slow_clk_counter <= 0;
            slow_clk <= ~slow_clk;
        end else begin
            slow_clk_counter <= slow_clk_counter + 1;
        end
    end

    // SoC
    soc soc0 (
        .sys_clk(sys_clk),
        .sys_rst(sys_rst),
        .vccio_clk(clk30),
        .led_clk(slow_clk),
        .user_leds_en(user_leds_en),
        .user_leds_color(user_leds_color),
        .vccio_en(vccio_en),
        .vccio_pdm(vccio_pdm),
        .spiflash_cs_n(spiflash_cs_n),
        .spiflash_mosi(spiflash_mosi),
        .spiflash_miso(spiflash_miso),
        .uart0_rx(syzygy0[0]),
        .uart0_tx(syzygy0[1]),
        .enc_a({syzygy0[20], syzygy0[22], syzygy0[24], syzygy0[26]}),
        .enc_b({syzygy0[21], syzygy0[23], syzygy0[25], syzygy0[27]}),
        .jtag_tck(syzygy0[2]),
        .jtag_tdi(syzygy0[4]),
        .jtag_tdo(syzygy0[6]),
        .jtag_tms(syzygy0[8])
    );

endmodule
