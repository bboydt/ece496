`include "ecp5_soc/utils/wishbone.v"

module encoder (
    input clk,
    input rst,

    input a,
    input b,

    output reg [31:0] position
);

    reg avg_a;
    reg avg_b;
    reg prev_a;
    reg [31:0] tmp_position;

    reg [47:0] a_samples;
    reg [47:0] b_samples;
    
    always @(posedge clk) begin
        // if rising edge
        if ((avg_a ^ prev_a) & avg_a) begin
            tmp_position <= (avg_b) ? tmp_position + 1 : tmp_position - 1;
        end
        prev_a <= avg_a;
        position <= (~rst) ? tmp_position : 32'd0;

        a_samples <= {a_samples[46:0], a};
        b_samples <= {b_samples[46:0], b};

        if (a_samples == 48'h0) begin
            avg_a <= 1'b0;
        end else if (a_samples == 48'hFFFFFFFFFF) begin
            avg_a <= 1'b1;
        end

        if (b_samples == 48'h0) begin
            avg_b <= 1'b0;
        end else if (b_samples == 48'hFFFFFFFFFF) begin
            avg_b <= 1'b1;
        end

    end

endmodule

module encoders_core (
    input clk,
    input rst,

    input [3:0] a,
    input [3:0] b,

    `WISHBONE_SLAVE(wb)

);

    wire [31:0] positions[3:0];

    encoder enc0 (
        .clk(clk),
        .rst(rst),
        .a(a[0]),
        .b(b[0]),
        .position(positions[0])
    );
    encoder enc1 (
        .clk(clk),
        .rst(rst),
        .a(a[1]),
        .b(b[1]),
        .position(positions[1])
    );
    encoder enc2 (
        .clk(clk),
        .rst(rst),
        .a(a[2]),
        .b(b[2]),
        .position(positions[2])
    );
    encoder enc3 (
        .clk(clk),
        .rst(rst),
        .a(a[3]),
        .b(b[3]),
        .position(positions[3])
    );
    
    always @(posedge clk) begin
        if (wb_cyc & wb_stb) begin
            if (~wb_ack & ~wb_err) begin
                if (wb_we) begin
                    wb_ack <= 1'b0;
                    wb_err <= 1'b1;
                end else begin
                    case (wb_adr[3:0])
                        32'h0: wb_miso <= positions[0];
                        32'h4: wb_miso <= positions[1];
                        32'h8: wb_miso <= positions[2];
                        32'hC: wb_miso <= positions[3];
                    endcase
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
