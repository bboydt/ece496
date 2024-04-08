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
    wire motor_pwm_clk;
    wire servo_pwm_clk;

    reg [10:0] clk_divider;
    always @(posedge clk30) begin
        clk_divider <= clk_divider + 1;
    end
    assign led_clk = clk_divider[7];
    assign motor_pwm_clk = clk_divider[0];
    assign servo_pwm_clk = clk_divider[3];

    assign reset_n = user_buttons[0];

    crg crg0 (
        .clk_in(clk30),
        .rst_in(~user_buttons[0]),
        .sys_clk(sys_clk),
        .sys_rst(sys_rst)
    );

    // Spiflash

    wire [63:0] gpio_out;

    wire [3:0] motor_dirs;
    assign syzygy0[19] = motor_dirs[3];
    assign syzygy0[17] = motor_dirs[2];
    assign syzygy0[15] = ~motor_dirs[1];
    assign syzygy0[13] = ~motor_dirs[0];

    // SoC
    soc soc0 (
        // Clock / Reset
        .sys_clk(sys_clk),
        .sys_rst(sys_rst),
        .vccio_clk(clk30),
        .led_clk(led_clk),
        .motor_pwm_clk(motor_pwm_clk),
        .servo_pwm_clk(servo_pwm_clk),

        // LEDs
        .user_leds_en(user_leds_en),
        .user_leds_color(user_leds_color),
        
        // SPIFlash
        .spiflash_cs_n(spiflash_cs_n),
        .spiflash_mosi(spiflash_mosi),
        .spiflash_miso(spiflash_miso),

        // Motors
        .motor_encs(syzygy0[27:20]),
        .motor_pwms({syzygy0[18], syzygy0[16], syzygy0[14], syzygy0[12]}),
        .motor_dirs(motor_dirs),

        // Servos
        //.servo_pwms({syzygy0[0], syzygy0[1]}),

        // VCCIO
        .vccio_en(vccio_en),
        .vccio_pdm(vccio_pdm),

        // UART
        .uart0_rx(syzygy0[1]),
        .uart0_tx(syzygy0[3]),
        .uart1_rx(syzygy0[5]),
        .uart1_tx(syzygy0[7]),

        // I2C
        .i2c_scl(syzygy0[4]),
        .i2c_sda(syzygy0[6]),

        // GPIO
        .gpio_out(gpio_out),

        // JTAG
        .jtag_tck(1'b0),
        .jtag_tdi(1'b0),
        .jtag_tms(1'b0)
        //
        //.jtag_tck(syzygy0[2]),
        //.jtag_tdi(syzygy0[4]),
        //.jtag_tdo(syzygy0[6]),
        //.jtag_tms(syzygy0[8])
    );

endmodule
