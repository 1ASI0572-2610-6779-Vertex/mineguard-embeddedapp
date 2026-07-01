#include "GPSSensor.h"
#include <Arduino.h>

GPSSensor::GPSSensor(int uartNum, int rx, int tx)
    : gpsSerial(uartNum),
      latitude(0.0),
      longitude(0.0),
      rxPin(rx),
      txPin(tx),
      locationFix(false) {
    gpsSerial.begin(9600, SERIAL_8N1, rxPin, txPin);
    Serial.println("GPS initialized!");
}

void GPSSensor::scanLocation() {
    while (gpsSerial.available()) {
        gps.encode(gpsSerial.read());
    }

    if (gps.location.isValid()) {
        latitude = gps.location.lat();
        longitude = gps.location.lng();
        locationFix = true;
    }
}

double GPSSensor::getLatitude() const {
    return latitude;
}

double GPSSensor::getLongitude() const {
    return longitude;
}

bool GPSSensor::hasFix() const {
    return locationFix;
}
