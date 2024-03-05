// Stream Macros
`define STREAM_WIRES(PREFIX) \
    wire PREFIX``_valid; \
    wire PREFIX``_ready;
`define STREAM_REGS(PREFIX) \
    reg PREFIX``_valid; \
    reg PREFIX``_ready;
`define STREAM_MASTER(PREFIX) \
    output reg PREFIX``_valid, \
    input PREFIX``_ready
`define STREAM_SLAVE(PREFIX) \
    input PREFIX``_valid, \
    output reg PREFIX``_ready
`define STREAM_PORT(PORT, PREFIX) \
    .PORT``_valid(PREFIX``_valid), \
    .PORT``_ready(PREFIX``_ready)


// Flow Macros
`define FLOW_WIRES(PREFIX) \
    wire PREFIX``_valid;
`define FLOW_REGS(PREFIX) \
    reg PREFIX``_valid;
`define FLOW_MASTER(PREFIX) \
    input reg PREFIX``_valid;
`define FLOW_SLAVE(PREFIX) \
    output reg PREFIX``_valid;
`define FLOW_PORT(PORT, PREFIX) \
    .PORT``_valid(PREFIX``_valid)


// IBus
`define IBUS_WIRES(PREFIX) \
    `STREAM_WIRES(PREFIX``_cmd); \
    wire [31:0] PREFIX``_cmd_pc; \
    `FLOW_WIRES(PREFIX``_rsp); \
    wire PREFIX``_rsp_error; \
    wire [31:0] PREFIX``_rsp_inst;
`define IBUS_REGS(PREFIX) \
    `STREAM_REGS(PREFIX``_cmd); \
    reg [31:0] PREFIX``_cmd_pc; \
    `FLOW_REGS(PREFIX``_rsp); \
    reg PREFIX``_rsp_error; \
    reg [31:0] PREFIX``_rsp_inst;
`define IBUS_PORT(PORT, PREFIX) \
    `STREAM_PORT(PORT``_cmd, PREFIX``_cmd), \
    .PORT``_cmd_pc(PREFIX``_cmd_pc), \
    `FLOR_PORT(PORT, PREFIX), \
    .PORT``_rsp_error(PREFIX``_rsp_error), \
    .PORT``_rsp_inst(PREFIX``_rsp_inst)
`define IBUS_MASTER(PREFIX) \
    `STREAM_MASTER(PREFIX``_cmd), \
    output reg [31:0] PREFIX``_cmd_pc, \
    `FLOW_SLAVE(PREFIX``_rsp), \
    input PREFIX``_rsp_error, \
    input [31:0] PREFIX``_rsp_inst
`define IBUS_SLAVE(PREFIX) \
    `STREAM_SLAVE(PREFIX``_cmd), \
    input [31:0] PREFIX``_cmd_pc, \
    `FLOW_MASTER(PREFIX``_rsp), \
    output reg PREFIX``_rsp_error, \
    output reg [31:0] PREFIX``_rsp_inst


// DBus
`define DBUS_WIRES(PREFIX) \
    `STREAM_WIRES(PREFIX``_cmd); \
    wire PREFIX``_cmd_wr; \
    wire [31:0] PREFIX``_cmd_address; \
    wire [31:0] PREFIX``_cmd_data; \
    wire [1:0] PREFIX``_cmd_size; \
    wire PREFIX``_rsp_ready; \
    wire PREFIX``_rsp_error; \
    wire [31:0] PREFIX``_rsp_data;
`define DBUS_REGS(PREFIX) \
    `STREAM_REGS(PREFIX``_cmd); \
    reg PREFIX``_cmd_wr; \
    reg [31:0] PREFIX``_cmd_address; \
    reg [31:0] PREFIX``_cmd_data; \
    reg [1:0] PREFIX``_cmd_size; \
    reg PREFIX``_rsp_ready; \
    reg PREFIX``_rsp_error; \
    reg [31:0] PREFIX``_rsp_data;
`define DBUS_PORT(PORT, PREFIX) \
    `STREAM_PORT(PORT``_cmd, PREFIX``_cmd), \
    .PORT``_cmd_wr(PREFIX``_cmd_wr), \
    .PORT``_cmd_address(PREFIX``_cmd_address), \
    .PORT``_cmd_data(PREFIX``_cmd_data), \
    .PORT``_cmd_size(PREFIX``_cmd_size), \
    .PORT``_rsp_ready(PREFIX``_rsp_ready), \
    .PORT``_rsp_error(PREFIX``_rsp_error), \
    .PORT``_rsp_data(PREFIX``_rsp_data)
`define DBUS_MASTER(PREFIX) \
    `STREAM_MASTER(PREFIX``_cmd), \
    output reg PREFIX``_cmd_wr, \
    output reg [31:0] PREFIX``_cmd_address, \
    output reg [31:0] PREFIX``_cmd_data, \
    output reg [1:0] PREFIX``_cmd_size, \
    input PREFIX``_rsp_ready, \
    input PREFIX``_rsp_error, \
    input [31:0] PREFIX``_rsp_data
`define DBUS_SLAVE(PREFIX) \
    `STREAM_SLAVE(PREFIX``_cmd), \
    input PREFIX``_cmd_wr, \
    input [31:0] PREFIX``_cmd_address, \
    input [31:0] PREFIX``_cmd_data, \
    input [1:0] PREFIX``_cmd_size, \
    output reg PREFIX``_rsp_ready, \
    output reg PREFIX``_rsp_error, \
    output reg [31:0] PREFIX``_rsp_data

