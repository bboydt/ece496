import os
import json

# Options
AddOption("--no-fw",
          dest = "firmware",
          action = "store_false",
          default = True,
          help = "Do not build firmware.")

AddOption("--no-gw",
          dest = "gateware",
          action = "store_false",
          default = True,
          help = "Do not build gateware.")

build_firmware = GetOption("firmware")
build_gateware = GetOption("gateware")



# Paths
root_dir = Dir(".")
deps_dir = Dir("deps")



# Base Environment
env = Environment(
    ENV = {
        "PATH": os.environ.get("PATH", ""),
    }
)

if "TERM" in os.environ:
    env["ENV"]["TERM"] = os.environ["TERM"]

# Build Firmware
if build_firmware:
    SConscript(
        dirs = "firmware",
        variant_dir = root_dir.Dir("build/fw"),
        duplicate = False,
        exports = {
            "env": env,
            "root_dir": root_dir,
            "deps_dir": deps_dir,
        }
    )

# Build Gateware
if build_gateware:
    SConscript(
        dirs = "gateware",
        variant_dir = root_dir.Dir("build/gw"),
        duplicate = False,
        exports = {
            "env": env,
            "root_dir": root_dir,
            "deps_dir": deps_dir,
            "bootrom": "#build/fw/app/bootrom/bootrom.init"
        }
    )

