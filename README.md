## Connect-5 Arduino Robot AI

This project is a hardware-integrated AI system that plays a variant of Connect-4 (Connect-5) on an  grid. It features a physical "dropping" mechanism controlled by servos and an IR-based sensing system to detect human moves. The "brain" is a C++ implementation of the Minimax algorithm with Alpha-Beta pruning, optimized for Arduino.

---

### ## Features

* **Minimax AI:** Look-ahead depth of up to 7 turns.
* **Heuristic Scoring:** Evaluates board states based on potential wins and threats.
* **Hardware Control:** Manages 8 servos for piece dispensing.
* **Real-time Sensing:** Uses 8 IR breakbeam sensors to detect human player input automatically.
* **Stuck Detection:** Logic to handle physical piece jams and ensure game continuity.
* **Visual Indicators:** LED feedback for turn tracking and win states.

---

### ## Components Required

* **Microcontroller:** Arduino Mega 2560 (Recommended due to RAM requirements for recursion).
* **Actuators:** 8x Micro Servos (e.g., SG90 or MG90S).
* **Sensors:** 8x IR Breakbeam Sensors.
* **Indicators:** 4x LEDs (2 for Player Turn/Win, 2 for AI Turn/Win).
* **Power:** 5V External Power Supply (Min 3A-5A for servos).

---

### ## Pin Mapping

| Component | Arduino Pins |
| --- | --- |
| **Servos (1-8)** | 37, 35, 33, 31, 29, 27, 25, 23 |
| **IR Breakbeams (1-8)** | 22, 24, 26, 28, 30, 32, 34, 36 |
| **Player LEDs** | 45, 46 |
| **AI LEDs** | 44, 47 |

---

### ## How it Works

#### 1. The Game Loop

The system alternates between `turn 1` (Human) and `turn 2` (AI).

* **Human Move:** The robot waits for an IR beam to be broken in any of the 8 columns. Once detected, it updates the internal board and switches turns.
* **AI Move:** The code runs a Minimax search to find the move that maximizes its score while minimizing yours. Once found, it activates the specific servo to drop a piece.

#### 2. The AI Logic

The AI uses a `fastHeuristic` function to score the board:

* **Win (5 in a row):** 100,000 points
* **Threat (4 in a row):** 10,000 points
* **Development (3 in a row):** 1,000 points
* **Blocked Lines:** 0 points (ignored)

#### 3. Mechanical Verification

The `moveServo` and `checkStuckPieces` functions work together to ensure the physical piece is released. If the IR beam is not triggered after a servo movement, the code attempts to "jitter" the servo to clear any jams.

---

### ## Setup Instructions

1. **Wiring:** Connect the servos and IR sensors to the pins specified in the code. Ensure a common ground between the Arduino and your external power supply.
2. **Calibration:** Adjust the `angles` array in the `moveServo` function. Every physical build is different; you must find the specific angle that opens your gate and the angle (12 degrees by default) that closes it.
3. **Deployment:** Upload the code to your Arduino Mega. The serial monitor will output data at 9600 baud for debugging.

---

### ## Troubleshooting

* **AI Crashes:** If using an Arduino Uno, the memory stack may overflow due to the `depth=7` recursion. Reduce depth to 3 or 4 for smaller boards.
* **Ghost Moves:** If the AI moves immediately after you, ensure your IR sensors are shielded from ambient sunlight.
* **Jams:** Check the `checkStuckPieces` logic; you may need to increase the `times` values if your pieces fall slowly.
