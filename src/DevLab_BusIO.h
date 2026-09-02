#ifndef DEVLAB_BUSIO_H
#define DEVLAB_BUSIO_H


#pragma once

template <typename T>

class DevLab_BusIO {

public:
    //Constructor
    DevLab_BusIO(T &bus) : _bus(bus) {};

    void begin();

    void update();

    T read();
    
    void write(T data);

    private:
    T _bus;
};

#endif // DEVLAB_BUSIO_H