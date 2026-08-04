*This project has been created as part of the 42 curriculum by <cramadan>.

# Codexion

## Description

Codexion is a multithreaded synchronization project written in C using POSIX threads (`pthread`).

The goal of this project is to simulate a group of coders competing for limited shared resources called dongles. Each coder must acquire two dongles before being allowed to compile. After compiling, the coder releases the resources and continues through debugging and refactoring phases.

The main challenge of the project is to correctly manage concurrent access to shared resources while preventing race conditions, deadlocks, starvation, and incorrect scheduling.

The project implements synchronization mechanisms to coordinate multiple threads and introduces scheduling strategies to decide which coder should receive resources first.

Implemented scheduling modes:

- **FIFO (First In First Out)**:
  Coders are served according to the time they requested resources.

- **EDF (Earliest Deadline First)**:
  Coders with the oldest compilation deadline receive priority.

The simulation ends when:
- A coder reaches burnout because they have waited too long without compiling.
- All coders complete the required number of compilations.

---

# Instructions

## Compilation

Compile the project using:

```bash
make