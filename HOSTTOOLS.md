Commands:
gen_secrets (gs): Generates secrets using the ectf25_design.gen_secrets command. Default parameters are "1 3 4".
build_decoder (bd): Builds the decoder using Docker. Default parameter is "0xdeadbeef".
gen_subscription (gb): Generates a subscription file subscription.bin. Default parameters are "0xdeadbeef 32 128 1".
firmware (firm): Flashes the insecure.bin firmware onto the max78000. Requires insecure.bin and OpenOCD.
flash: Flashes the decoder build binary to a device. Uses OpenOCD if available, otherwise uses Python tools.
list (ls): Lists subscribed channels on the decoder.
subscribe (sub): Updates subscriptions using subscription.bin.
tester (test): Tests frame decoding on the decoder. Default parameters are "rand -c 1 -f 64".
uplink (up): Starts the ectf.uplink command, broadcasting to port 2000.
satellite (sat): Starts the ectf.satellite command, broadcasting to TV 1 on port 2001.
tv: Sends encoded frames to a decoder and returns the decoded result. Default parameter is "2001".
py (python): Runs a Python command in the ectf virtual environment. Default parameter is "-m pip list".
oo (openocd): Starts an OpenOCD debugging session with the max78000.
gdb: Starts a GDB debugging session, connecting to any running OpenOCD sessions.
Switches:
-h, --help: Displays help information.
-p, --parameters: Overrides default parameters for a command.
-b, --bindir: Temporarily changes the directory where the decoder binary is stored.
-s, --serialport: Temporarily changes the location of the serial device port.