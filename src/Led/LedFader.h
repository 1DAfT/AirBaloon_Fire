#ifndef LED_FADER_H
#define LED_FADER_H

#include <Arduino.h>

class LedFader
{
private:
    int _pin;          // Пин, к подключен светодиод
    int _interval; // Интервал шага изменения ШИМ
    int _brightness; // Яркость в данный момент
    int _minBrightness; // Время последнего переключения
    int _maxBrightness;    // Текущее состояние светодиода
    bool _fading; // Фалаг состояния мерцания
    int _inverter = 1;
public:
    LedFader(int pin, int interval, int min, int max);
    ~LedFader();

    unsigned long _fadeIntervalTime; //время смены интервала

    void initLed();
    void setBrightness(int brightness);
    void setMaxBrightness();
    void fadeToMin();
    void startFading(int maxBrightness = 0, int minBrightness = 0);
    int getPin();
    int getBrightness();
    int getInverter();
};

#endif