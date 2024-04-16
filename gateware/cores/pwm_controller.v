module pwm_controller (
    input sys_clk,
    input sys_rst,
    input pwm_clk,
    output reg pwm,
    `WISHBONE_SLAVE(wb)
);

    reg [15:0] pwm_compare;
    reg [15:0] pwm_counter;
    always @(posedge pwm_clk) begin 
        pwm_counter <= (pwm_counter == 16'hFFFE) ? (16'h0) : (pwm_counter + 1);
        pwm <= (pwm_counter < pwm_compare);
    end

    always @(posedge sys_clk) begin
        if (wb_cyc & wb_stb) begin
            if (~wb_ack & ~wb_err) begin
                if (wb_we) begin
                    if (wb_adr[3:0] == 4'h0) begin
                        pwm_compare <= wb_mosi[0+:16];
                    end
                    wb_ack <= 1'b1;
                    wb_err <= 1'b0;
                end else begin
                    if (wb_adr[3:0] == 4'h0) begin
                        wb_miso <= {16'h0, pwm_compare};
                    end
                    wb_ack <= 1'b1;
                    wb_err <= 1'b0;
                end
            end
        end else begin
            wb_ack <= 1'b0;
            wb_err <= 1'b0;
        end
    end
    

endmodule

