import os
import json

# Paths
#

deps_dir = Dir("deps")
ecp5_soc_dir = deps_dir.Dir("ecp5-soc")
neorv32_dir = ecp5_soc_dir.Dir("deps/neorv32")
rt_dir = Dir("/Users/boyd/Me/rt") # @nocheckin testing rt
firmware_dir = Dir("firmware")


# Base Environment
#

env = Environment(
    ENV = {
        "PATH": os.environ.get("PATH", ""),
    }
)

if "TERM" in os.environ:
    env["ENV"]["TERM"] = os.environ["TERM"]



# Gateware Environment
#

yosys_flags = [
    "-q",
]

nextpnr_flags = [ 
    "--um5g-85k",
    "--speed=8", 
    "--package=CABGA381",
    "--lpf=board/butterstick_r1_0.pcf"
]

ecppack_flags = [
    "--compress",
    "--freq=38.8",
]

gw_env = env.Clone(
    tools = ["yosys", "nextpnr", "trellis", "icarus", "cc"],
    toolpath = ["deps/scons-fpga"],
    GHDL = "ghdl",
    VFLAGS = "-sv",
    YOSYSFLAGS = yosys_flags,
    PNRFLAGS = nextpnr_flags,
    ECPPACKFLAGS = ecppack_flags,
)



# Neorv32
#
# Neorv32 is written in VHDL so we need to build a verilog version of it

# In the future we might want to customize the processor, but for now just use ecp5-soc's wrapper.
neorv32_wrapper_src = File("rtl/neorv32.vhd")
neorv32_wrapper = SConscript(
    ecp5_soc_dir.File("deps/SConscript-neorv32"),
    variant_dir = "build/gateware/neorv32",
    duplicate = False,
    exports = {
        "env": gw_env,
        "neorv32_dir": neorv32_dir,
        "neorv32_wrapper_src": neorv32_wrapper_src
    }
)



# Top Design
#

top = SConscript(
    dirs = "rtl",
    variant_dir = "build/gateware/top",
    duplicate = False,
    exports = {
        "env": gw_env,
        "ecp5_soc_dir": ecp5_soc_dir,
        "neorv32_wrapper": neorv32_wrapper,
    }
)

top_ast = top["ast"]
top_rom_init = top["init_files"]["ROM0"]
top_textcfg = gw_env.Ecp5Pnr(top_ast)



# Firmware Libraries
#

libstartrt = SConscript(
    firmware_dir.File("start/SConscript"),
    variant_dir = "build/firmware/start_rt",
    duplicate = False,
    exports = {
        "env": fw_env
    }
)


libstart = SConscript(
    firmware_dir.File("start_nort/SConscript"),
    variant_dir = "build/firmware/start",
    duplicate = False,
    exports = {
        "env": fw_env
    }
)

libneorv32 = SConscript(
    deps_dir.File("SConscript-libneorv32"),
    variant_dir = "build/firmware/neorv32",
    duplicate = False,
    exports = {
        "env": fw_env,
        "neorv32_dir": neorv32_dir
    }
)

librt = SConscript(
    rt_dir.File("src/SConscript"),
    variant_dir = "build/firmware/rt",
    duplicate = False,
    exports = {
        "env": fw_env
    }
)

librt_riscv = SConscript(
    dirs = rt_dir.Dir("arch/riscv"),
    variant_dir = "build/firmware/rt/riscv",
    duplicate = False,
    exports = {
        "env": fw_env
    }
)

bootrom = SConscript(
    firmware_dir.File("bootrom/SConscript"),
    variant_dir = "build/firmware/bootrom",
    duplicate = False,
    exports = {
        "env": fw_env,
        "fw_shared_dir": Dir("firmware/shared")
    }
)


# RT Blinky
app_rt_env = fw_env.Clone()
app_rt_env.Append(LIBS = [libstartrt, libneorv32, librt, librt_riscv])
blinky = SConscript(
    firmware_dir.File("blinky/SConscript"),
    variant_dir = "build/firmware/blinky",
    duplicate = False,
    exports = {
        "env": app_rt_env,
        "fw_shared_dir": Dir("firmware/shared")
    }
)
fw_env.DfuSuffix("build/firmware/blinky/blinky.dfu", blinky)



# Milestone 1
app_env = fw_env.Clone()
app_env.Append(LIBS = [libstart, libneorv32])
m1 = SConscript(
    firmware_dir.File("m1/SConscript"),
    variant_dir = "build/firmware/m1",
    duplicate = False,
    exports = {
        "env": app_env,
        "fw_shared_dir": Dir("firmware/shared")
    }
)
fw_env.DfuSuffix("build/firmware/m1/m1.dfu", m1)




# Bitstreams
#

top_bootrom_textcfg = env.Command(
    "build/gateware/top/top_bootrom.config",
    [top_textcfg, top_rom_init, bootrom],
    "ecpbram -i ${SOURCES[0]} -o $TARGET -f ${SOURCES[1]} -t ${SOURCES[2]}"
)
top_bitstream = gw_env.Ecp5Bitstream(top_bootrom_textcfg)
fw_env.DfuSuffix("build/gateware/top/top.dfu", top_bitstream)

