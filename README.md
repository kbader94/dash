# OpenDash

OpenDash is a Qt-based infotainment center for your Linux OpenAuto installation!
The OpenDash project includes OpenAuto, AASDK, and Dash.

Main features of Dash include:

*	Embedded OpenAuto `Windowed/Fullscreen`
*	Wireless OpenAuto Capability
*	On-screen Volume, Brightness, & Theme Control
*	Responsive Scalable UI `Adjustable for screen size`
*	Bluetooth Media Control
*	Real-Time Vehicle OBD-II Data & SocketCAN Capabilities
*	Theming `Dark/Light mode` `Customizable RGB Accent Color`
*	True Raspberry Pi 7” Official Touchscreen Brightness Control
*	App-Launcher built in
*	Camera Access `Streaming/Local` `Backup` `Dash`
*	Keyboard Shortcuts `GPIO Triggerable`

![](docs/imgs/opendash-ui.gif)

# Getting Started

## Video walk through
_steps may be slightly different such as ia (intelligent-auto) has been renamed to dash, the UI has changed, etc..._

https://youtu.be/CIdEN2JNAzw

## Install Script

Dash can be built automatically utilizing an included script.

The install script included in the dash repo will install all the required packages and compile all portions of the OpenDash project.

### 1. Clone the repo, Run the install script
```
git clone https://github.com/openDsh/dash

cd dash

./install.sh
```
# HOME LAUNCHER

The new Home launcher is meant to basically turn Open Dash into a full linux DE.
It features an "Android style" app launcher that automatically lists all apps on your system.
It's capable of launching the apps, switching between them, and ending them when needed. 
Unfortunately due to the low level requirements of a DE, this launcher is tightly integrated and currently only works
with KDE's KWIN window manager. Additionally it currently depends on wmctrl to close windows, though this is just temporary.

## Launcher Installation & Use
1) Install KDE on your system and install wmctrl.
2) Launch into a KDE session, install OpenDash via the install instructions above.
3) Select the Home Launcher from the launcher settings dialog.

### The Home Launcher modifies the KWIN window rules to set the size and position of windows within open dash
### To revert back to your system's original settings, open the KDE window rules setting app and delete the rule named dash


