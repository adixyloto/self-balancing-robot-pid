# Self-Balancing Robot — PID Control Prototype

A two-wheeled self-balancing robot prototype built using an Arduino UNO, MPU6050 IMU, L298N motor driver and PID-based feedback control.

> **Project status:** Prototype / experimental  
> The system demonstrated active motor correction and partial stabilization during testing, but sustained autonomous balancing was not fully achieved.

---

## Overview

The objective of this project was to develop a two-wheeled robot capable of correcting its tilt using sensor feedback and closed-loop control.

The robot uses an MPU6050 inertial measurement unit to estimate its tilt angle. This angle is compared with a predefined upright setpoint, and a PID controller generates a correction signal that determines the direction and speed of the motors.

The project involved both hardware integration and iterative controller tuning.

---

## System Architecture

```text
        MPU6050 IMU
             │
             │ Tilt Angle
             ▼
      ┌───────────────┐
      │ PID Controller│
      └───────┬───────┘
              │
        Correction Output
              │
              ▼
      ┌───────────────┐
      │ Motor Control │
      │   PWM + DIR   │
      └───────┬───────┘
              │
              ▼
       L298N Motor Driver
          │          │
          ▼          ▼
      Left Motor  Right Motor
          │          │
          └────┬─────┘
               ▼
          Robot Motion
               │
               └──────► Tilt changes
                         │
                         └──► Feedback
