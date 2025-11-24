# Responsive LED Pattern Controller

This project implements a dual-mode LED lighting controller for an 8-LED array using the Maxim Integrated (Analog Devices) microcontroller SDK. 

The key feature of this system is its **responsive input handling**. Unlike standard blink programs that freeze during delays, this program continuously polls the user button even while executing lighting animations, ensuring immediate pattern switching.

## 🛠 Hardware Configuration

### Pin Mapping
The code is configured for the following GPIO mappings:

| Component | GPIO Port | Pin | Description |
|-----------|-----------|-----|-------------|
| **Button**| `GPIO2`   | `3` | Input (Pull-up enabled) |
| **LED 1** | `GPIO1`   | `6` | Output |
| **LED 2** | `GPIO0`   | `9` | Output |
| **LED 3** | `GPIO0`   | `8` | Output |
| **LED 4** | `GPIO0`   | `11`| Output |
| **LED 5** | `GPIO0`   | `19`| Output |
| **LED 6** | `GPIO3`   | `1` | Output |
| **LED 7** | `GPIO0`   | `16`| Output |
| **LED 8** | `GPIO0`   | `17`| Output |

## 💡 Operating Modes

The system toggles between two distinct patterns when the button is pressed.

### Mode 0: Odd/Even Alternation
* **Description:** Alternates between lighting up all even-indexed LEDs and all odd-indexed LEDs.
* **Timing:** switches every 400ms.

### Mode 1: Larson Scanner (Knight Rider)
* **Description:** A single LED "bounces" back and forth from LED 1 to LED 8.
* **Timing:** 120ms per frame.

## 🧠 Software Logic

### The "Responsive Delay" Strategy
To prevent the system from becoming unresponsive during LED delays, the standard `MXC_Delay` is wrapped in a custom function:

```c
int delay_with_button_check(int total_ms);