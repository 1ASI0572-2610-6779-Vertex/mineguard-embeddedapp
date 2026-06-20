#ifndef MODEST_IOT_H
#define MODEST_IOT_H

/**
 * @file ModestIoT.h
 * @brief Convenience header for the Modest IoT Nano-framework (C++ Edition),
 *        configured for the MineGuard embedded application.
 *
 * Core framework by Angel Velasquez (CC BY-ND 4.0). The MineGuard-specific
 * sensors and actuators below extend the framework's Sensor/Actuator bases.
 */

// --- Framework core ---
#include "EventHandler.h"
#include "CommandHandler.h"
#include "Sensor.h"
#include "Actuator.h"
#include "Device.h"

// --- MineGuard sensors ---
#include "UltrasonicSensor.h"
#include "PulseSensor.h"
#include "CollisionSensor.h"
#include "EmergencyButton.h"
#include "GPSSensor.h"

// --- MineGuard actuators ---
#include "Led.h"
#include "Buzzer.h"
#include "LcdDisplay.h"

#endif // MODEST_IOT_H
