# EQ10Q LV2 Plugin Bundle
EQ10Q is a powerful and flexible suite of audio plugins built for the LV2 standard. What started as a specialized parametric equalizer has evolved into a comprehensive bundle of professional-grade DSP tools including compression, gating, and expansion.

All audio processing is written in C and highly optimized for low CPU load without compromising audio quality. The internal engine utilizes 64-bit floating point math, while interfacing with LV2 ports via standard 32-bit floats. Every plugin in the bundle features a custom Graphical User Interface (GUI) written in C++ and Gtkmm for precise, intuitive control.

## 🚀 Included Plugins
### EQ10Q
A 10-band parametric equalizer featuring a real-time frequency response plot.

Total Control: Gain, Frequency, and Q controls for every band and Mid/Side mode for the stereo version.

Intuitive Interaction: Use mouse dragging on the curve, scroll wheel for Q, or double-click for numeric entry.

Advanced Features: A/B comparison buttons, per-band bypass, and integrated VU-meters for input/output.

Versatile Filters:

* LPF/HPF: 1st, 2nd, 3rd, and 4th order (6 to 24 dB/oct).

* Shelving: High and Low Shelving filters.

* Peaking: Analog-matched peaking algorithms designed for natural response.

* Notch: Precise frequency rejection.

### CS10Q
A high-performance Compressor designed for everything from subtle leveling to aggressive punch.

Side-chain Support: Includes an optional external side-chain input and an internal side-chain filter to prevent specific frequencies from triggering the compressor.

### GT10Q
A versatile Gate/Expander plugin sharing the same high-quality DSP core as the CS10Q.

Dynamic Control: Effectively eliminate noise or expand the dynamic range of your tracks.

Side-chain Filtering: Features side-chain input and filtering options to ensure the gate only opens when the desired signal trigger is present.

## 🛠 Installation
Prerequisites
Ensure you have the following libraries and tools installed:
* Compiler: G++
* Build System: CMake, pkg-config
* Libraries: 
  * LV2 Headers
  * Gtkmm >= 2.4
  * FFTW3

### Building from Source
Navigate to the EQ10Q top-level directory:
Bash
cmake ./

Compile the source:

Bash
make
Install as root:

Bash
sudo make install
The plugin installs to /usr/local/lib/lv2 by default.

## 📋 Requirements
To run these plugins, you need an LV2-compliant host. We recommend:

* Ardour
* Carla
* Harrison Mixbus

## 🗑 Uninstallation
To remove the plugins from your system, delete the installation folder (default location):

Bash
sudo rm -rf /usr/local/lib/lv2/sapistaEQv2.lv2

### 🤝 Contributing & License
EQ10Q is released under the GPL License. You are free to copy, redistribute, and modify the software.
