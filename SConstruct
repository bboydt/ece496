import os

yosys_flags = [
    "-q",
]

nextpnr_flags = [ 
    "--um5g-85k",
    "--speed=8", 
    "--package=CABGA381",
    "--lpf=butterstick_r1_0.pcf"
]

ecppack_flags = [
    "--compress",
    "--freq=38.8",
]

env = Environment(
    tools = ["yosys", "nextpnr", "trellis", "icarus"],
    toolpath = ["deps/scons-fpga"],

    ENV = {
        "PATH": os.environ.get("PATH", ""),
        "TERM": os.environ.get("TERM", ""),
    },

    GHDL = "ghdl",
    VFLAGS = "-sv",
    YOSYSFLAGS = yosys_flags,
    PNRFLAGS = nextpnr_flags,
    ECPPACKFLAGS = ecppack_flags,
)

# Paths
#

deps_dir = Dir("deps")
ecp5_soc_dir = deps_dir.Dir("ecp5-soc")
neorv32_dir = ecp5_soc_dir.Dir("deps/neorv32")

# Neorv32
#
# Neorv32 is written in VHDL so we need to build a verilog version of it

# In the future we might want to customize the processor, but for now just use ecp5-soc's wrapper.
neorv32_wrapper_src = ecp5_soc_dir.File("rtl/ecp5_soc/cores/neorv32.vhdl")
neorv32_wrapper = SConscript(
    ecp5_soc_dir.File("deps/SConscript-neorv32"),
    variant_dir = "build/neorv32",
    duplicate = False,
    exports = {
        "env": env,
        "neorv32_dir": neorv32_dir,
        "neorv32_wrapper_src": neorv32_wrapper_src
    }
)

# Top Design
#

top_ast = SConscript(
    dirs = "rtl",
    variant_dir = "build/rtl",
    duplicate = False,
    exports = {
        "env": env,
        "ecp5_soc_dir": ecp5_soc_dir,
        "neorv32_wrapper": neorv32_wrapper
    }
)

top_textcfg = env.Ecp5Pnr(top_ast)

# This bitstream will have a random rom, so use ecpbram to replace it.
top_bitstream = env.Ecp5Bitstream(top_textcfg)
