import os

env = Environment(
    tools = ["yosys", "nextpnr", "trellis", "icarus"],
    toolpath = ["deps/scons-fpga"],

    ENV = {
        "PATH": os.environ.get("PATH", ""),
        "TERM": os.environ.get("TERM", ""),
    },

    GHDL = "ghdl",
    VFLAGS = "-sv",

    YOSYSFLAGS = "-q",
)

neorv32_wrapper = SConscript(
    "deps/ecp5-soc/deps/SConscript-neorv32",
    variant_dir = "build/ecp5-soc/neorv32",
    duplicate = False,
    exports = {
        "env": env,
        "ROOT_DIR": Dir("deps/ecp5-soc")
    }
)

soc_base = SConscript(
    dirs = "deps/ecp5-soc/rtl",
    variant_dir = "build/ecp5-soc",
    duplicate = False,
    exports = {
        "env": env,
        "neorv32_wrapper": neorv32_wrapper
    }
)
