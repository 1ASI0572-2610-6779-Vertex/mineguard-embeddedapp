#ifndef GPS_SENSOR_H
#define GPS_SENSOR_H

#include <TinyGPSPlus.h>
#include <HardwareSerial.h>

class GPSSensor {
private:
    TinyGPSPlus gps;
    HardwareSerial gpsSerial;
    double latitude;
    double longitude;
    int rxPin;
    int txPin;
    bool locationFix;

public:
    GPSSensor(int uartNum, int rx, int tx);

    void scanLocation();
    double getLatitude() const;
    double getLongitude() const;
    bool hasFix() const;
};

#endif //GPS_SENSOR_H
