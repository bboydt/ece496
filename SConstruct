import os

nextpnr_flags = [ 
#    "-q",
    "--um5g-85k",
    "--speed=8", 
    "--package=CABGA381",
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
    VDEFINES = {
        "ROM0_LENGTH": 16*1024,
        "BRAM0_LENGTH": 16*1024
    },

    YOSYSFLAGS = "-q",
    PNRFLAGS = [
        "--um5g-85k",
        "--speed=8", 
        "--package=CABGA381",
        "--lpf=butterstick_r1_0.pcf"
    ],

    ECPPACKFLAGS = [
        "--compress",
        "--freq=38.8",
    ]
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

ast = SConscript(
    dirs = "rtl",
    variant_dir = "build/rtl",
    duplicate = False,
    exports = {
        "env": env,
        "soc_base": soc_base
    }
)

textcfg = env.Ecp5Pnr(ast)
bitstream = env.Ecp5Bitstream(textcfg)
