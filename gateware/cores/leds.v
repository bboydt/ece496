module leds #(
    parameter COUNT = 7
) (
    input sys_clk,
    input sys_rst,
    `WISHBONE_SLAVE(wb),

    input led_clk,
    output reg [COUNT-1:0] leds,
    output reg [2:0] rgb_sel
);
    // Wishbone
    reg [31:0] colors[COUNT-1:0];
    localparam LED_BITS = $clog2(COUNT);
    always @(posedge sys_clk) begin
        if (wb_cyc & wb_stb) begin
            if (~wb_ack & ~wb_err) begin
                if (wb_we) begin
                    colors[wb_adr[2+:LED_BITS]] <= wb_mosi;
                    wb_ack <= 1'b1;
                    wb_err <= 1'b0;
                end else begin
                    wb_miso <= colors[wb_adr[2+:LED_BITS]];
                    wb_ack <= 1'b1;
                    wb_err <= 1'b0;
                end
            end
        end else begin
            wb_ack <= 1'b0;
            wb_err <= 1'b0;
        end
    end


    // PWM Generation

    reg red_clk;
    reg green_clk;
    reg blue_clk;
    wire [COUNT-1:0] leds_red;
    wire [COUNT-1:0] leds_green;
    wire [COUNT-1:0] leds_blue;

    genvar i;
    generate
        for (i = 0; i < COUNT; i++) begin
            pwm #(
                .WIDTH(8)
            ) led_red_pwm (
                .clk(red_clk),
                .rst(sys_rst),
                .max(8'hff),
                .threshold(colors[i][16+:8]),
                .out(leds_red[i])
            );
            pwm #(
                .WIDTH(8)
            ) led_green_pwm (
                .clk(green_clk),
                .rst(sys_rst),
                .max(8'hff),
                .threshold(colors[i][8+:8]),
                .out(leds_green[i])
            );
            pwm #(
                .WIDTH(8)
            ) led_blue_pwm (
                .clk(blue_clk),
                .rst(sys_rst),
                .max(8'hff),
                .threshold(colors[i][0+:8]),
                .out(leds_blue[i])
            );
        end
    endgenerate

    // Color Selection Logic

    localparam STATE_RED = 2'd0,
               STATE_GREEN = 2'd1,
               STATE_BLUE = 2'd2;
    
    reg [2:0] state;
    
    always @(posedge led_clk) begin
        case (state)
            STATE_RED: begin
                rgb_sel <= 3'b001;
                leds <= leds_red;
                red_clk <= 1;
                green_clk <= 0;
                blue_clk <= 0;
                state <= STATE_GREEN;
            end
            STATE_GREEN: begin
                rgb_sel <= 3'b100;
                leds <= leds_green;
                red_clk <= 0;
                green_clk <= 1;
                blue_clk <= 0;
                state <= STATE_BLUE;
            end
            STATE_BLUE: begin
                rgb_sel <= 3'b010;
                leds <= leds_blue;
                red_clk <= 0;
                green_clk <= 0;
                blue_clk <= 1;
                state <= STATE_RED;
            end
            default: state <= STATE_RED;
        endcase
    end

endmodule
