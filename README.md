# Tomasulo Algorithm Simulator

This is a **C++ implementation of a Tomasulo Algorithm Simulator**, supporting dynamic scheduling for floating-point instructions with reservation stations (RS), register renaming (RAT), a common data bus (CDB), and a reorder buffer (ROB). This simulator mimics out-of-order execution with data hazard resolution.

---

## 🚀 Features

* Supports basic arithmetic instructions: `ADD`, `SUB`, `MUL`, `DIV` (case-insensitive).
* Simulates:

  * **Reservation Stations (RS)** for `ADD/SUB` and `MUL/DIV`.
  * **Register Alias Table (RAT)** for register renaming.
  * **Register File (RF)** for register values.
  * **Reorder Buffer (ROB)** to manage out-of-order commit.
  * **ALUs** that simulate instruction latency.
* Exception handling for division by zero.
* Tracks the full pipeline: **Issue → Dispatch → Write Back → Commit**.
* Cycle-by-cycle state visualization of RF, RAT, RS, ALUs, and ROB.

---

## 📂 Input Format

All instructions are read from a file named `ques.txt`.

Each instruction should follow the format:

```
<OPERATION> <DEST>,<SRC1>,<SRC2>
```

Example:

```
ADD F1,F2,F3
SUB F2,F1,10
MUL F3,F2,F1
DIV F4,F3,F1
```

* Operands can be registers (`F1`–`F5`) or immediates (only for SRC2).
* Registers must be named `F1` through `F5`.

---

## ⚙️ Usage

1. **Prepare input file**: Place your instructions in `ques.txt` following the format above.

2. **Compile and Run**:

```bash
g++ tomasulo.cpp -o tomasulo
./tomasulo
```

3. **Program will prompt you to enter**:

   * RS size for ADD/SUB
   * RS size for MUL/DIV
   * Execution cycles for ADD, SUB, MUL, and DIV
   * Initial values for registers F1–F5

4. **Simulator Output**:

   * Prints cycle-by-cycle internal state (RF, RAT, RS, ROB, ALU buffers)
   * Shows instruction progress through each stage
   * Final summary includes per-instruction timings

---

## 🧠 Implementation Details

* **Register File (RF)**: Stores the committed values of registers `F1` to `F5`.

* **Register Alias Table (RAT)**: Maps each register to either a RS tag (e.g., `RS1`) or empty string if ready.

* **Reservation Stations (RS)**:

  * Two types: `RSA` for ADD/SUB and `RSM` for MUL/DIV.
  * Tracks operands (values or RS tags), operator, and assigned instruction.

* **Reorder Buffer (ROB)**:

  * Each issued instruction reserves a ROB entry.
  * Manages instruction ordering and commitment.

* **ALU Units**:

  * Two ALU buffers simulate ADD/SUB and MUL/DIV execution.
  * Each ALU executes one instruction at a time with specified latency.

* **Instruction Lifecycle**:

  * **Issue**: Finds available RS and ROB entry, sets operands.
  * **Dispatch**: Moves ready instructions to ALU if operands are available.
  * **Write Result**: ALU broadcasts result to dependent RS and updates ROB.
  * **Commit**: Writes result to RF if it's the head of ROB.

---

## 🚩 Exception Handling

* Division-by-zero will be detected and printed as:

```
Exception happened in cycle <cycle>
```

* Simulation stops on exception.

---

## 📊 Sample Output

```
_____________________Cycle 3_____________________

	  ______RF_______
     F1  |         4   |
     F2  |         5   |
     F3  |         0   |
     F4  |         0   |
     F5  |         0   |
	  _______________

	  ___RAT___
     F1  |  RS1  |
     F2  |       |
     F3  |       |
     F4  |       |
     F5  |       |
 	  _________

	  ________RS(ADD/SUB)________
     RS1  |   + |        4 |        5 |
     RS2  |     |          |          |
	  ___________________________

	BUFFER:  (RS1) 4 + 5
...

issue   dispatch  write back  commit
  1         2          4         5
  2         3          5         6
...
```

---

## 📌 Notes

* Only supports registers `F1`–`F5`.
* Input must not contain syntax errors or unsupported instructions.
* Results are shown in the console; optionally, you can uncomment `#define cout outfile` to write to `ans.txt`.
