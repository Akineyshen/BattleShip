# Battle Ship ❤️
This program is designed for `STM32L476RGTx` microcontrollers and implements a version of the "Battleship" game, leveraging both the LCD display for graphical output and UART for input and interaction with the user. The game consists of two main components: the LCD display that shows the game's state and allows the player to position their ship, and the UART interface that enables the opponent (or the player) to guess the ship's location by issuing commands.

# **How the Game Works:**
1. `Initialization:` The program starts by initializing the microcontroller's hardware components, including GPIO pins for the LCD interface, the ADC (Analog-to-Digital Converter) for reading analog inputs, and the UART for communication. It also initializes the game's internal state, such as the game board represented as a two-dimensional array.

2. `LCD Display:` The game utilizes a custom character set to represent different elements of the game on the LCD, such as the ship, water, and hit or miss markers. The player uses buttons connected to the ADC input to navigate a cursor on the LCD for placing their ship at the beginning of the game.

3. `Gameplay:`
   * The game loop awaits input from the UART, expecting commands that represent the opponent's guesses. Commands are formatted as "shoot x y", where x and y denote the coordinates on the game board.
   * The player's ship's position is adjusted using the LCD's buttons, and the position can be changed between rounds based on the rules defined in the game logic.
   * Hits and misses are tracked, and the LCD display is updated accordingly to show the game's progress. Custom characters on the LCD represent the outcomes of each guess (hit or miss) visually.
  
4. `Winning Condition:` The game proceeds in rounds, with the opponent having a limited number of guesses (attempts) each round to find the ship. The game continues until the ship is hit, at which point the program announces the game's end and displays a message on both the LCD and via UART.
   
5. `System Interface:` The program features a simple text-based menu displayed over UART at the beginning, guiding the user to start the game. It also provides real-time feedback during the game, such as indicating whether the opponent's guess was too far left, right, or in line with the ship's position.

6. `Error Handling and ADC Input:` The program includes basic error handling for invalid inputs and uses the ADC to read analog values from the buttons. These values are interpreted to control the game's logic, such as navigating the cursor on the LCD or confirming selections.

This program showcases the integration of various STM32 microcontroller peripherals (LCD, UART, ADC) to create an interactive game. It demonstrates handling of digital and analog inputs, character-based LCD manipulation, and serial communication, providing a comprehensive example of embedded systems programming on the STM32 platform.
