# Group 6's RISCV OS Project

This project is designed to be able to run a multithreaded OS for a small gaming system. Our demo is
a small game modeled after Space Invaders. The user controls a small ship moving left to right with the
'a' and 'd' keys, and presses the 'u' key to fire bullets at a group of aliens while dodging bullets from
the aliens. The game is over when the player is hit 3 times or the aliens are all destroyed.

## Our OS instructions
The exes should be up to date, but in case they aren't, simply run `make` in the `riscv-cart` and the
`riscv-firmware` dirs. The binaries for both will be in their respective `/bin` dirs. Once the game is loaded,
press 'CMD' to start.

![How the game should look](gameplay.png)

### For the TA

We successfully implemented the entirety of our API. We have a few known bugs which we were unable to resolve but are seen only in extreme circumstances. It is unlikely that you will observe an of these bugs within our game or other groups ports to our game, but we felt obligated to share for completion

- malloc / free will corrupt memory locations outside of the heap when run thousands of times, even if we free everything we write

- mutex's can lock get stuck when switching between threads
   - We provided a workaround by turning off timer interrupts during the time which a mutex is locked
   - May cause an issue if yield() or wait() is called within a protected section

- In our game, sometimes the ship with go haywire (rarely seen)
   - Possibly an error with the event handler

## Group 7 OS Port
We only partially succeeded with Group 7's OS powering our game. The starting screen prints okay, but once
'CMD' is pressed the screen turns black and the OS appears to be stuck in the interrupt function indefinitely. 
We tried to be faithful to the API that was provided in the Dev Kit, but it seemed incomplete in a lot of ways.
The code is located within the dir `group-7-cart` and the firmware binary is in the same dir as the Makefile.

## Group 8 OS Port
Group 8's API seems to match ours very well and is well thought out, but we never received any header files
to implement their API so we cannot test our game on their OS. We made a good-faith implementation based on
the API documentation that was provided. The code is located within the dir `group8-riscv-cart` and the 
firmware binary is in the same dir as the Makefile.