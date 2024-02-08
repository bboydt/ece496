import os
import json

# Paths
#

deps_dir = Dir("deps")
ecp5_soc_dir = deps_dir.Dir("ecp5-soc")
neorv32_dir = ecp5_soc_dir.Dir("deps/neorv32")
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
    "--lpf=butterstick_r1_0.pcf"
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



# Firmware Environment
#

gnu_flags = [
    "-Wall",
    "-march=rv32imac_zicsr_zifencei",
    "-mabi=ilp32",
    "-O0",
    "-ffunction-sections",
    "-fdata-sections",
    "-flto",
    "-nostartfiles",
    "-mno-fdiv",
    "-mstrict-align",
    "-mbranch-cost=10",
    "-nostdlib",
    "-Wl,--gc-sections"
]

fw_env = env.Clone(
    CC = "riscv64-unknown-elf-gcc",
    AS = "riscv64-unknown-elf-gcc",
    LINK = "riscv64-unknown-elf-gcc",
    AR = "riscv64-unknown-elf-gcc-ar",
    RANLIB = "riscv64-unknown-elf-gcc-ranlib",
    OBJCOPY = "riscv64-unknown-elf-objcopy",

    CPPPATH = [
        Dir("firmware/shared/include").srcnode(),
        neorv32_dir.Dir("sw/lib/include").srcnode(),
    ],
    CPPDEFINES = {"RT_CYCLE_ENABLE": 1},
    CCFLAGS = gnu_flags,
    ASFLAGS = gnu_flags,
    CFLAGS = ["-std=gnu17"],
    CXXFLAGS = [
        "-std = gnu++23",
        "-Wno-missing-field-initializers",
        "-fno-threadsafe-statics",
        "-fno-exceptions",
    ],
    LINKFLAGS = gnu_flags,
    LINKCOM = "$LINK -o ${TARGETS[0]} -Wl,-Map,${TARGETS[1]} $LINKFLAGS $SOURCES"
    "$_LIBDIRFLAGS -Wl,--start-group $_LIBFLAGS -Wl,--end-group",
    PROGSUFFIX = ".elf",

    # printing
    CCCOMSTR     = "cc $TARGET",
    CXXCOMSTR    = "c++ $TARGET",
    ASCOMSTR     = "as $TARGET",
    ASPPCOMSTR   = "as $TARGET",
    LINKCOMSTR   = "ld $TARGET",
    ARCOMSTR     = "ar $TARGET",
    RANLIBCOMSTR = "ranlib $TARGET",
)

env.Append(
    CPPSUFFIXES = [".s"]
)

ld_scripts = [File("firmware/shared/memory.ld"), File("firmware/shared/sections_boot.ld")]
ld_script_flags = [f"-Wl,-T,{f.path}" for f in ld_scripts]

def ld_emitter(target, source, env):
    map_file = os.path.splitext(str(target[0]))[0] + ".map"
    Depends(target, ld_scripts)
    return target + [map_file], source

fw_env.Append(
    LINKFLAGS = ld_script_flags,
    PROGEMITTER = ld_emitter
)



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
        "env": gw_env,
        "neorv32_dir": neorv32_dir,
        "neorv32_wrapper_src": neorv32_wrapper_src
    }
)



# Top Design
#

top = SConscript(
    dirs = "rtl",
    variant_dir = "build/rtl",
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

libstart = SConscript(
    firmware_dir.File("start/SConscript"),
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

boot_env = fw_env.Clone(
    LIBS = [libstart, libneorv32]
)

blinky = SConscript(
    firmware_dir.File("blinky/SConscript"),
    variant_dir = "build/firmware/blinky",
    duplicate = False,
    exports = {
        "env": boot_env
    }
)



# Bitstreams
#

blinky_textcfg = env.Command(
    "build/blinky.config",
    [top_textcfg, top_rom_init, blinky],
    "ecpbram -i ${SOURCES[0]} -o $TARGET -f ${SOURCES[1]} -t ${SOURCES[2]}"
)

blinky_bitstream = gw_env.Ecp5Bitstream(blinky_textcfg)

