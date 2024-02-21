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
        xip_miso : in std_ulogic
    );
end entity;

architecture neorv32_wrapper_rtl of neorv32_wrapper is

begin

    neorv32_top_inst: neorv32_top
    generic map (
        CLOCK_FREQUENCY => 30_000_000,
        INT_BOOTLOADER_EN => false,
        
        CPU_EXTENSION_RISCV_A => true,
        CPU_EXTENSION_RISCV_C => true,
        CPU_EXTENSION_RISCV_M => true,
        CPU_EXTENSION_RISCV_Zicntr => true,
        
        MEM_INT_IMEM_EN => false,
        MEM_INT_DMEM_EN => false,
        
        IO_GPIO_NUM => 7, 
        IO_MTIME_EN => true,
        IO_UART0_EN => true,
        IO_XIP_EN => true,
        
        MEM_EXT_EN => true,

        
        IO_UART0_RX_FIFO => 64,
        IO_UART0_TX_FIFO => 64

    )
    port map (
        clk_i  => sys_clk,
        rstn_i => sys_rst_n,

        wb_cyc_o => bus_cyc,
        wb_stb_o => bus_stb,
        wb_we_o  => bus_we,
        wb_ack_i => bus_ack,
        wb_err_i => bus_err,
        wb_tag_o => bus_tag,
        wb_sel_o => bus_sel,
        wb_adr_o => bus_adr,
        wb_dat_o => bus_mosi,
        wb_dat_i => bus_miso,

        uart0_txd_o => uart0_tx,
        uart0_rxd_i => uart0_rx,

        gpio_o => gpio_out,

        xip_csn_o => xip_csn,
        xip_clk_o => xip_clk,
        xip_dat_i => xip_miso,
        xip_dat_o => xip_mosi
    );

end architecture;

