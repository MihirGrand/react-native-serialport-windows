#pragma once
#include "pch.h"
#include <string>
#include <Windows.h>
#include <functional>
#include <thread>
#include <atomic>

class SerialPort {
public:
    enum class DataBits { Five = 5, Six = 6, Seven = 7, Eight = 8 };
    enum class StopBits { One = 1, OnePointFive = 0, Two = 2 };
    enum class Parity { None = 0, Odd = 1, Even = 2 };
    enum class FlowControl { None = 0, Software = 1, Hardware = 2 };

    using DataReceivedCallback = std::function<void(const std::vector<uint8_t>&)>;

    SerialPort(const std::string& portName);
    ~SerialPort();

    bool open(int baudRate, DataBits dataBits, StopBits stopBits, 
             Parity parity, FlowControl flowControl);
    void close();
    bool write(const std::vector<uint8_t>& data);
    void setDataReceivedCallback(DataReceivedCallback callback);
    bool isOpen() const { return m_handle != INVALID_HANDLE_VALUE; }

private:
    void startReading();
    void readThread();
    void stopReading();

    std::string m_portName;
    HANDLE m_handle;
    std::atomic<bool> m_isRunning;
    std::thread m_readThread;
    DataReceivedCallback m_dataCallback;
    bool m_isReading;
};