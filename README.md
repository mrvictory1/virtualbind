# virtualbind
Keybinder for Waydroid. Work in progress, expect bugs and limitations.
## Installing
Installation is currently not available, see below section for compiling. Virtualbind does not require any modifications to Waydroid or additional Android applications installed on Waydroid.
## Compiling
Dependencies are gcc, make and development package of libevdev. On Fedora, `libevdev-devel` provides necessary development files. Simply running `make` from root directory will compile the program. Compiled program will be available in the root directory with name `virtualbind`.
## Running and Configuration
Virtualbind needs configuration files in order to run. Please refer to [Wiki](https://github.com/mrvictory1/virtualbind/wiki) for more information.
## Roadmap / TODO
### v0.1: Robustness
* Add support for all keyboard keys
* Add error handling
* Add automatic input device detection
* Remove limitations of configuration files
### v0.2: Daemons & Publishing
* Create persistent privileged daemon that launches virtualbind and stops it when needed
* Create persistent user daemon that connects to Waydroid
* Create AUR PKGBUILD
* Create Fedora Copr repo
* Create PPA for Ubuntu
### v1.0
* GUI
### v1.1
* Context-dependent automatic profile switching

