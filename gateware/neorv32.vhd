library ieee;
use ieee.std_logic_1164.all;

library neorv32;
use neorv32.neorv32_package.all;

entity neorv32_wrapper is
    port (
        sys_clk : in std_ulogic;
        sys_rst_n : in std_ulogic;

        uart0_tx : out std_ulogic;
        uart0_rx : in std_ulogic;
        uart1_tx : out std_ulogic;
        uart1_rx : in std_ulogic;

        twi_sda_i : in std_ulogic;
        twi_sda_o : out std_ulogic;
        twi_scl_i : in std_ulogic;
        twi_scl_o : out std_ulogic;

        gpio_out : out std_ulogic_vector(63 downto 0);

        bus_cyc : out std_ulogic;
        bus_stb : out std_ulogic;
        bus_we  : out std_ulogic;
        bus_tag : out std_ulogic_vector(2 downto 0);
        bus_sel : out std_ulogic_vector(3 downto 0);
        bus_adr : out std_ulogic_vector(31 downto 0);
        bus_mosi : out std_ulogic_vector(31 downto 0);
        bus_miso : in  std_ulogic_vector(31 downto 0);
        bus_ack : in  std_ulogic;
        bus_err : in  std_ulogic;

        xip_csn : out std_ulogic;
        xip_clk : out std_ulogic;
        xip_mosi : out std_ulogic;
        xip_miso : in std_ulogic;

        jtag_trst : in std_ulogic;
        jtag_tck : in std_ulogic;
        jtag_tdi : in std_ulogic;
        jtag_tdo : out std_ulogic;
        jtag_tms : in std_ulogic
    );
end entity;

architecture neorv32_wrapper_rtl of neorv32_wrapper is

begin

    neorv32_top_inst: neorv32_top
    generic map (
        CLOCK_FREQUENCY => 80_000_000,

        INT_BOOTLOADER_EN => false,
        ON_CHIP_DEBUGGER_EN => true,

        CPU_EXTENSION_RISCV_A => false,
        CPU_EXTENSION_RISCV_C => true,
        CPU_EXTENSION_RISCV_M => true,
        CPU_EXTENSION_RISCV_Zicntr => true,
        
        MEM_INT_IMEM_EN => false,
        MEM_INT_DMEM_EN => true,
        MEM_INT_DMEM_SIZE => 65536,
        
        XBUS_EN => true,

        XIP_EN => true,
        XIP_CACHE_EN => true,
        
        IO_GPIO_NUM => 32, 

        IO_MTIME_EN => true,

        IO_TWI_EN => true,
        IO_TWI_FIFO => 8,
        
        IO_UART0_EN => true,
        IO_UART0_RX_FIFO => 64,
        IO_UART0_TX_FIFO => 64,

        IO_UART1_EN => true,
        IO_UART1_RX_FIFO => 64,
        IO_UART1_TX_FIFO => 64
    )
    port map (
        clk_i  => sys_clk,
        rstn_i => sys_rst_n,

        xbus_cyc_o => bus_cyc,
        xbus_stb_o => bus_stb,
        xbus_we_o  => bus_we,
        xbus_ack_i => bus_ack,
        xbus_err_i => bus_err,
        xbus_sel_o => bus_sel,
        xbus_adr_o => bus_adr,
        xbus_dat_o => bus_mosi,
        xbus_dat_i => bus_miso,

        uart0_txd_o => uart0_tx,
        uart0_rxd_i => uart0_rx,
        uart1_txd_o => uart1_tx,
        uart1_rxd_i => uart1_rx,

        twi_sda_i => twi_sda_i,
        twi_sda_o => twi_sda_o,
        twi_scl_i => twi_scl_i,
        twi_scl_o => twi_scl_o,

        gpio_o => gpio_out,

        xip_csn_o => xip_csn,
        xip_clk_o => xip_clk,
        xip_dat_i => xip_miso,
        xip_dat_o => xip_mosi,

        jtag_trst_i => jtag_trst,
        jtag_tck_i  => jtag_tck,
        jtag_tdi_i  => jtag_tdi,
        jtag_tdo_o  => jtag_tdo,
        jtag_tms_i  => jtag_tms

    );

end architecture;

