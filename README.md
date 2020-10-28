These programs were made as homeworks, therefore, these are my codes written according to given assignments, which were not my idea.


  32-bit CPU - a simple processor processing arithmetic operations (only cpu.c and cpu.h are written by me):

  The program accepts two to three arguments: the first is run or trace, the second is optional stackCapacity, and the last is the path to the instruction file (binary file (code in assembler converted to binary form)). Run executes all instructions and prints the CPU state, trace prints the state after each instruction and waits for Enter before executing the next one.
  

  Texas Hold'em Poker:

  Cards will be represented by a pair of characters: the first determines the value: 2, 3, 4, 5, 6, 7, 8, 9, T, J, Q, K, A; second color: h, d, s, c for ♥, ♦, ♠, ♣. 
  The input instances will consist of three lines:
    - two cards of one player,
    - two cards of the other player,
    - five cards lying on the table
  The program does not end immediately after resolving a single instance, but only when there is EOF on the input.
  The program responds for each instance with:
    W if the first player wins,
    D if there is a draw,
    L if the first player lost.


  Ludo - an easy "game":
  
  The program for the "game" function prints a random game (it shifts by the number of squares in the playing field thrown on the dice, you must roll a six to start, you can roll another six for the next roll) for a given length of the field and the probability of a six roll. The "average game" function also returns the average game length (number of rounds) for a given field length, the number of games and the probability of a six roll.
  
