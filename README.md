# Shut the Box

Terminal version of the classic dice game, written in C++.

## Rules

You have 9 tiles numbered 1–9, all starting open. Each turn, roll the dice and close any combination of open tiles that sums to your roll. The game ends when no valid combination exists. Your score is the sum of the remaining open tiles — lower is better. Close all tiles for a perfect score of 0.

#### Classic
- Roll **2 dice** while any tile above 6 is still open
- Switch to **1 die** once only tiles 1–6 remain

### Endless
- You can toggle the tiles instead of just closing them.
- Always roll **2 dice**.

## Build & Run

```bash
g++ main.cpp -o main
./main
```

## Input

Type tile numbers separated by spaces and press Enter:

```
Rolled: [4][3]  =  7
Choose tiles to close: 3 4
```
