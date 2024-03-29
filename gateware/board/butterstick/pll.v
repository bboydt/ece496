// diamond 3.7 accepts this PLL
// diamond 3.8-3.9 is untested
// diamond 3.10 or higher is likely to abort with error about unable to use feedback signal
// cause of this could be from wrong CPHASE/FPHASE parameters
module ecp5_pll
(
    input clk30, // 30 MHz, 0 deg
    output clk90, // 80 MHz, 0 deg
    output clk1, // 1 MHz, 0 deg
    output clk250k, // 0.0204806 MHz, 0 deg
    output locked
);
(* FREQUENCY_PIN_CLKI="30" *)
(* FREQUENCY_PIN_CLKOP="80" *)
(* FREQUENCY_PIN_CLKOS="1" *)
(* FREQUENCY_PIN_CLKOS2="0.0204806" *)
(* ICP_CURRENT="12" *) (* LPF_RESISTOR="8" *) (* MFG_ENABLE_FILTEROPAMP="1" *) (* MFG_GMCREF_SEL="2" *)
EHXPLLL #(
        .PLLRST_ENA("DISABLED"),
        .INTFB_WAKE("DISABLED"),
        .STDBY_ENABLE("DISABLED"),
        .DPHASE_SOURCE("DISABLED"),
        .OUTDIVIDER_MUXA("DIVA"),
        .OUTDIVIDER_MUXB("DIVB"),
        .OUTDIVIDER_MUXC("DIVC"),
        .OUTDIVIDER_MUXD("DIVD"),
        .CLKI_DIV(3),
        .CLKOP_ENABLE("ENABLED"),
        .CLKOP_DIV(7),
        .CLKOP_CPHASE(3),
        .CLKOP_FPHASE(0),
        .CLKOS_ENABLE("ENABLED"),
        .CLKOS_DIV(560),
        .CLKOS_CPHASE(3),
        .CLKOS_FPHASE(0),
        .CLKOS2_ENABLE("ENABLED"),
        .CLKOS2_DIV(27343),
        .CLKOS2_CPHASE(3),
        .CLKOS2_FPHASE(0),
        .FEEDBK_PATH("CLKOP"),
        .CLKFB_DIV(8)
    ) pll_i (
        .RST(1'b0),
        .STDBY(1'b0),
        .CLKI(clk30),
        .CLKOP(clk90),
        .CLKOS(clk1),
        .CLKOS2(clk250k),
        .CLKFB(clk90),
        .CLKINTFB(),
        .PHASESEL0(1'b0),
        .PHASESEL1(1'b0),
        .PHASEDIR(1'b1),
        .PHASESTEP(1'b1),
        .PHASELOADREG(1'b1),
        .PLLWAKESYNC(1'b0),
        .ENCLKOP(1'b0),
        .LOCK(locked)
	);
endmodule
