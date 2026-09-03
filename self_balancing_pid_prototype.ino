#include <Wire.h>
#include <MPU6050.h>
#include <math.h>

MPU6050 mpu;

// ===== PID PARAMETERS =====
float Kp = 35.0;
float Ki = 0.0;
float Kd = 1.5;
float setpoint = -0.75;

// ===== MOTOR PINS =====
#define IN1 7
#define IN2 6
#define IN3 5
#define IN4 4
#define ENA 9
#define ENB 10

// ===== MOTOR TRIM =====
#define LEFT_TRIM  15
#define RIGHT_TRIM 0

float integral = 0;
float lastError = 0;
unsigned long lastTime = 0;

void setup() {
  Serial.begin(9600);

  Wire.begin();
  mpu.initialize();

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  stopMotors();

  Serial.println("Balancing start...");
  delay(2000);
}

void loop() {
  unsigned long now = millis();
  float dt = (now - lastTime) / 1000.0;

  // Run control loop at approximately 100 Hz
  if (dt < 0.01) {
    return;
  }

  lastTime = now;

  // ===== READ MPU6050 =====
  int16_t ax, ay, az;
  int16_t gx, gy, gz;

  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  // Estimate tilt angle from accelerometer
  float angle = atan2((float)ay, (float)az) * 180.0 / PI;

  // ===== PID CONTROL =====
  float error = angle - setpoint;

  integral += error * dt;
  integral = constrain(integral, -50, 50);  // Anti-windup

  float derivative = (error - lastError) / dt;
  lastError = error;

  float output =
      (Kp * error) +
      (Ki * integral) +
      (Kd * derivative);

  output = constrain(output, -255, 255);

  // ===== SAFETY CUTOFF =====
  if (abs(error) > 30) {
    stopMotors();
    integral = 0;

    Serial.println("FALLEN - motors off");
    return;
  }

  // ===== MOTOR CONTROL =====
  driveMotors(output);

  // Serial monitoring for tuning
  Serial.print("A:");
  Serial.print(angle);

  Serial.print(" E:");
  Serial.print(error);

  Serial.print(" O:");
  Serial.println(output);
}

void driveMotors(float output) {

  int speed = abs(output);

  int leftSpeed =
      constrain(speed + LEFT_TRIM, 0, 255);

  int rightSpeed =
      constrain(speed + RIGHT_TRIM, 0, 255);

  if (output > 0) {

    // Forward
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);

  } else {

    // Backward
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);

    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  }

  analogWrite(ENA, leftSpeed);
  analogWrite(ENB, rightSpeed);
}

void stopMotors() {

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}
