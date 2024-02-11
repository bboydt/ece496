module led_controller #(
    parameter LED_COUNT = 7
) (
    input sys_clk,
    input sys_rst,
    `WISHBONE_SLAVE(wb),

    input pwm_clk,
    output reg [2:0] leds_color,
    output reg [6:0] leds_en
);
    localparam RED_INDEX = 2'd2;
    localparam GREEN_INDEX = 2'd1;
    localparam BLUE_INDEX = 2'd0;

    reg [7:0] color_counter[2:0];
    reg [24*LED_COUNT-1:0] led_values;

    integer i, j;

    // Generate color strobe
    //
    
    reg [2:0] color_index;
    always @(posedge pwm_clk) begin
        if (color_index < 3) begin
            color_index <= color_index + 1;
        end else begin
            color_index <= 0;
        end

        for (i = 0; i < 3; i++) begin
            color_counter[i] <= color_counter[i] + leds_color[i];
        end

        case (color_index)
            RED_INDEX: begin
                leds_color <= 3'b001;
            end
            GREEN_INDEX: begin
                leds_color <= 3'b100;
            end
            BLUE_INDEX: begin
                leds_color <= 3'b010;
            end
        endcase

        for (i = 0; i < LED_COUNT; i++) begin
            if (color_counter[color_index] > 8'hff - led_values[24*i+8*color_index+:8]) begin
                leds_en[i] <= 1'b1;
            end else begin
                leds_en[i] <= 1'b0;
            end
        end
    end







    initial begin
        led_values = {
            24'h00FF00,
            24'h00D800,
            24'h00B400,
            24'h009000,
            24'h006C00,
            24'h004800,
            24'h002400
        };
    end

endmodule
