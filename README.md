# Smart Elevator System 

## Overview
This project is a smart elevator system for a four-floor building using Arduino. It can handle multiple floor requests and move efficiently using queue and direction-based logic.

---

## Features
- Multiple button request handling  
- Queue-based system  
- Direction-based movement  
- Automatic door control  
- LCD display for status  

---

## Components
- Arduino Uno  
- L293D Motor Driver  
- DC Motor  
- Servo Motor  
- I2C LCD  
- Push Buttons  

---

## 🔌 Pin Configuration

### Motor Driver (L293D)
| Function | Arduino Pin |
|--------|------------|
| ENA    | 9          |
| IN1    | 8          |
| IN2    | 7          |

---

### Servo Motor (Door)
| Function | Arduino Pin |
|--------|------------|
| Signal | 6          |

---

### Push Buttons
| Floor | Arduino Pin |
|------|------------|
| Floor 1 | 2 |
| Floor 2 | 3 |
| Floor 3 | 4 |
| Floor 4 | 5 |

---

### Buzzer
| Function | Arduino Pin |
|--------|------------|
| Buzzer | 10 |

---

### I2C LCD
| Function | Arduino Pin |
|--------|------------|
| SDA | A4 |
| SCL | A5 |

---

## Working Principle
- User presses floor buttons  
- Requests are stored in a queue  
- System waits briefly to collect multiple inputs  
- Elevator moves in optimized direction  
- Stops at requested floors and opens door  

---

## How to Run
1. Upload the code to Arduino  
2. Connect components according to the pin configuration  
3. Power the system  
4. Press buttons to simulate elevator operation  

---

## Demo Video

---

## Author
Md. Samiul Islam Sabbir
