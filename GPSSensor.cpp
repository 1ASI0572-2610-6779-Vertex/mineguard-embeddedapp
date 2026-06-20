#include "GPSSensor.h"
#include <Arduino.h>

GPSSensor::GPSSensor(int uartNum, int rx, int tx, EventHandler* eventHandler)
    : Sensor(uartNum, eventHandler), gpsSerial(uartNum), rxPin(rx), txPin(tx),
      latitude(0.0), longitude(0.0) {
    gpsSerial.begin(9600, SERIAL_8N1, rxPin, txPin); //16, 17
    Serial.println("GPS initialized!");
}

void GPSSensor::scanLocation() {
    while (gpsSerial.available()) {
        gps.encode(gpsSerial.read());
    }

    latitude = gps.location.lat(), 6;
    longitude = gps.location.lng(), 6;
}

double GPSSensor::getLatitude() const {
    return latitude;
}

double GPSSensor::getLongitude() const {
    return longitude;
}