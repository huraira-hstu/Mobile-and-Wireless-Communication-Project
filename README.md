
# Mobile and Wireless Communication Project

## Installation and Running

If ESP-IDF is not already installed on your system, follow the steps below to install and configure the framework.

### Installing ESP-IDF

Clone the official ESP-IDF repository:

```bash
git clone --recursive https://github.com/espressif/esp-idf.git
```

Navigate into the repository:

```bash
cd esp-idf
```

Run the installation and export scripts according to the shell you are using.

Run the installation script according to the shell you are using:

**PowerShell:**
```powershell
.\install.ps1
```

**Command Prompt:**
```cmd
install.bat
```

**Bash:**
```bash
./install.sh
```

**Fish:**
```fish
./install.fish
```

Wait for the installation to finish successfully. After that, run the corresponding export script to set up the ESP-IDF environment:

**PowerShell:**
```powershell
.\export.ps1
```

**Command Prompt:**
```cmd
export.bat
```

**Bash:**
```bash
./export.sh
```

**Fish:**
```fish
./export.fish
```


The export script must be run whenever you open a new terminal session before using ESP-IDF commands.

### Running This Project

Clone this repository:

```bash
git clone https://github.com/huraira-hstu/Mobile-and-Wireless-Communication-Project.git
```

Navigate into the project directory:

```bash
cd Mobile-and-Wireless-Communication-Project
```

#### Set the Chip Target

Set the target chip for the project:

```bash
idf.py set-target <target>
```

For example, to use the ESP32 DevKit V1:

```bash
idf.py set-target esp32
```

Make sure your ESP-IDF installation supports the selected target. You can check the available targets with:

```bash
idf.py --list-targets
```

#### Configure Project Settings

Use menuconfig to configure chip-specific settings:

```bash
idf.py menuconfig
```

For example, the default `sdkconfig` may specify a flash size of 2 MB. If your board has 4 MB of flash, you can change it through:

`Serial flasher config → Flash size`

Alternatively, comment out the following line in `sdkconfig`:

```ini
CONFIG_ESPTOOLPY_FLASHSIZE_2MB=y
```
and add the following line

```ini
CONFIG_ESPTOOLPY_FLASHSIZE_4MB=y
```

Make sure the configured flash size matches your actual board.

#### Build the Project

Compile the project:

```bash
idf.py build
```

#### Flash the ESP32

Connect your ESP32 board and flash the firmware:

```bash
idf.py flash
```
Hold the boot button on esp32 during flash.

#### Monitor Serial Output

Start the serial monitor:

```bash
idf.py -p <PORT> monitor
```

Replace `<PORT>` with the serial port assigned to your ESP32.

- **Windows:** Check Device Manager under `Ports (COM & LPT)` (e.g., `COM3`).
- **Linux:** Run `ls /dev/ttyUSB* /dev/ttyACM*` (e.g., `/dev/ttyUSB0` or `/dev/ttyACM0`).
 
You can also build, flash, and monitor the project in one command:

```bash
idf.py -p <PORT> flash monitor
```

To exit the serial monitor, press `Ctrl + ]`.
