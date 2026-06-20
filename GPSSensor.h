#ifndef GPS_SENSOR_H
#define GPS_SENSOR_H

#include "Sensor.h"
#include <TinyGPSPlus.h>
#include <HardwareSerial.h>

class GPSSensor : public Sensor {
private:
    TinyGPSPlus gps;
    HardwareSerial gpsSerial;
    double latitude;
    double longitude;
    int rxPin;
    int txPin;

public:
    GPSSensor(int uartNum, int rx, int tx, EventHandler* eventHandler = nullptr);

    void scanLocation();
    double getLatitude() const;
    double getLongitude() const;
};

#endif //GPS_SENSOR_H
