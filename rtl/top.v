module top (
    input clk30,
    input [1:0] user_buttons,
    output [6:0] user_leds_en,
    output [2:0] user_leds_color
);

    assign user_leds_color = 3'b110;
    assign user_leds_en = {user_buttons[1], {3{~user_buttons[1], user_buttons[1]}}};

endmodule
