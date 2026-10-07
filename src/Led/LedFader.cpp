#include "LedFader.h"

LedFader::LedFader(int pin, int interval, int min, int max)
{
    _pin = pin;
    _interval = interval;
    _minBrightness = min;
    _maxBrightness = max;
    _fadeIntervalTime = 0;
}

LedFader::~LedFader()
{
}

void LedFader::initLed(){
    pinMode(_pin, OUTPUT);   
}

void LedFader::setBrightness(int brightness){
    _brightness = brightness;
    analogWrite(_pin, brightness); 
}

void LedFader::setMaxBrightness(){
    _brightness = _maxBrightness;
    analogWrite(_pin, _maxBrightness); 
}

void LedFader::startFading(int maxBrightness = 0, int minBrightness = 0){
    int newBrightness = _brightness + (_interval * _inverter);
    
    int MAX = (maxBrightness == 0) ? _maxBrightness : maxBrightness;
    int MIN = (minBrightness == 0) ? _minBrightness : minBrightness;

    if ((newBrightness < MIN && _brightness >= MIN ) 
    || (newBrightness > MAX && _brightness <= MAX) 
    || newBrightness < 0 
    || newBrightness > 255){
        _inverter = -_inverter;
    }

    _brightness = _brightness + (_interval * _inverter);
    analogWrite(_pin, _brightness);
}

void LedFader::fadeToMin(){
    if(_brightness > _minBrightness){
        _brightness = _brightness - _interval;
    }
    if(_brightness < 0){
        analogWrite(_pin, 0);
    } else {
        analogWrite(_pin, _brightness);
    }
}

int LedFader::getBrightness(){
    return _brightness;
}

int LedFader::getInverter(){
    return _inverter;
};

int LedFader::getPin(){
    return _pin;
}