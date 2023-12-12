# Group Members
Aditya Kumar Bej, Shivani Kalamadi, Farhad Alemi, Rafael A Diaz Cruz

# Group 6 Firmware
You can find the firmware under "firmware" folder.

# Group 6 Cartridge
You can find the catridge under "cartridge" folder. We have developed the "snake" game. Upload the firmware and upload the cartridge

# Setup Instructions
Run the "make" command in both the firmware and cartridge folder to generate the .strip file for each.
Run the riscv-sim by the ./runsim.sh script. Upload the firmware code and cartridge code

# Gameplay instructions
You can move the snake with the w, a, x, d keys. CMD button pauses and unpauses the game. Hit the RST button to reset the game.
When you hit the walls or accidentally eat your own body, you die. The speed and the length of the snake increases every time you eat the food. Please note: this is a hostile alien planet, day and night last only 10 seconds.

# Final requirements
• Launch the application program when cartridge is entered - Done.
• Access multi-button controller input - Done through a,w,d,x.
• React to periodic timer - Done through background colour change from blue to black every 10 seconds.
• React to video interrupt - Done through moving the snake and pausing it through CMD.
• Draw to the background, large, medium and small sprites - Done, please refer api.h in our cartridge.

**********IMPORTANT NOTE**********
Please note - This code has been tested on the 7a0019e commit version of the risc-v simulator (https://github.com/UCDClassNitta/riscv-console/tree/7a0019e176f57dc0312361aa495856209b902896)