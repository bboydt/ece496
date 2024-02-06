module top (
    input clk30,
    input [1:0] user_buttons,
    output reg [2:0] user_leds_color,
    output reg [6:0] user_leds_en
);
 
    reg en;
    reg [2:0] color;
    reg [32:0] counter;

    always @(*) begin
        user_leds_en = {en, {3{~en, en}}};
        user_leds_color = color;
    end

    always @(posedge clk30) begin
        counter <= counter + 1;

        if (counter >= 15000000) begin
            counter <= 0;
            en <= ~en;
            color <= color + 1;
        end
    end
    
endmodule

