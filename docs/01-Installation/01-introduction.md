### Introduction
While previous Prism Studio versions relied on Prism Product Manager for plugin installation and license activation,
there are now simple installers for the free plugins, with the installation steps outlined below.

If you have ever installed a Prism Studio product on Windows or macOS before, you will need to first run the included 
Prism Cleaner utility to clear out old installations that may conflict with this new release.

---

### Requirements
The requirements for running all Prism Studio plug-ins are listed below. All plug-ins are in the VST3 format and
must be run within a supporting DAW.

#### Windows System Requirements
* `Windows 10 64 bit`
* `See your DAW's minimum system specs`

#### macOS System Requirements
* `macOS 64 bit (Intel or M1/M2)` version 10.15.7 or later
* `See your DAW's minimum system specs`

_**NOTE**: macOS native DAWs such as **Logic** often only support AudioUnit plug-ins natively, but you can use a VST3 wrapper product to
utilize Prism Studio tools within them. Alternatively, we suggest continuing your music production in your macOS
DAW of choice, and then utilizing **Reaper**, **Tracktion Waveform**, **Ableton Live**, or another VST3 supporting DAW to do
a visuals pass on your bounced track using Prism Studio tools._

---

### Installation
Please download the Prism Studio zip package for your operating system: [Windows](https://github.com/Lumasonic/Lumasonic/releases/download/prism-studio-v1.0.2/PrismStudio_win.zip)
or [macOS](https://github.com/Lumasonic/Lumasonic/releases/download/prism-studio-v1.0.2/PrismStudio_macOS.zip).

Unzip the package and first run `PrismCleanerSetup`. Then follow the instructions in the next section below to run the Prism Cleaner utility.

Next, run `PrismStudioSetup` to install all Prism Studio plug-ins to the standard VST3 location for your OS (this will include `Prism`, `Prism Audio Bus`,
`Prism Monitor`, and `Prism Reference`).

Follow the instructions laid out in the [Prism Studio](../02-Prism-Studio/01-introduction.md) docs to finish the configuration in your DAW.

---

### Prism Cleaner
Prism Cleaner is a command line utility tool that clears out all files related to previous installations of
Prism Studio and related products. This ensures a clean starting point for installing and using the latest plugins.

---

#### Getting Started

Start by closing any DAW that you may have open. This is required to clear out all existing Prism plug-ins.

When launched, Prism Cleaner will explain all of the steps it is about to take to clean your system of all 
Cymatic Somatics software. To accept the default and clean everything, simply press Enter/Return at each prompt. 

For examples on Windows and macOS, please see their respective sections below.

**IMPORTANT**: If you receive a warning during any of these steps, simply follow the onscreen instructions. After
Prism Cleaner is finished running, the log file `PrismCleaner.log` will be created on your desktop for further
investigation.

Prism Cleaner will walk through the following prompts:
1. Remove Prism Studio and all of its associated files?
2. Remove all Lumasonic content & playlist files?
3. Remove Lumasonic Media Server and all of its associated files?
4. Remove Prism Product Manager and all of its associated files?
5. Remove all Cymatic Somatics global directories and files?

This will ensure that ANY previous installations of Prism Product Manager, Lumasonic Media Server (and content), and
any Prism Studio tools are completely cleared.

#### Windows

For Windows, find Prism Cleaner in the Start menu (either by searching by name or looking through `All apps`) and 
run it.

![PrismCleanerWin image-center image-full image-margin-v-24](img/Prism_Cleaner_win_1.png)

The dialog will walk you through the process. Simply close the window if you want to cancel at any point.

![PrismCleanerWinRun image-center image-full image-margin-v-24](img/Prism_Cleaner_win_2.png)

#### macOS

Open a Terminal window, type `sudo /Applications/PrismCleaner`, and hit Enter/Return to run Prism Cleaner. 
Enter your password to begin the process, and you will be walked through the steps in the same fashion as the above
Windows example.

At any point you can exit the process by pressing `Ctrl+C` on your keyboard

![PrismCleanerMac image-center image-full image-margin-v-24](img/Prism_Cleaner_macOS_1.png)

---

**[NEXT: Prism Studio Introduction](../02-Prism-Studio/01-introduction.md)**