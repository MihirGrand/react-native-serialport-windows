#include "pch.h"
#include <windows.h>
#include "SerialPort.h"

SerialPort::SerialPort(const std::string& portName)
    : m_portName(portName)
    , m_handle(INVALID_HANDLE_VALUE)
    , m_isRunning(false)
    , m_isReading(false) {
}

SerialPort::~SerialPort() {
    close();
}
void applyFlowControl(DCB& dcb, SerialPort::FlowControl flowControl) {
    switch (flowControl) {
        case SerialPort::FlowControl::None:
            dcb.fOutxCtsFlow = FALSE;
            dcb.fOutX = FALSE;
            dcb.fInX = FALSE;
            break;
        case SerialPort::FlowControl::Software:
            dcb.fOutxCtsFlow = FALSE;
            dcb.fOutX = TRUE;
            dcb.fInX = TRUE;
            break;
        case SerialPort::FlowControl::Hardware:
            dcb.fOutxCtsFlow = TRUE;
            dcb.fOutX = FALSE;
            dcb.fInX = FALSE;
            break;
    }
}
bool SerialPort::open(int baudRate, SerialPort::DataBits dataBits, SerialPort::StopBits stopBits, 
                     SerialPort::Parity parity, SerialPort::FlowControl flowControl) {
    if (m_handle != INVALID_HANDLE_VALUE) {
        close();
    }

    m_handle = CreateFileA(
        m_portName.c_str(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        nullptr
    );

    if (m_handle == INVALID_HANDLE_VALUE) {
        return false;
    }

    DCB dcb = { 0 };
    dcb.DCBlength = sizeof(DCB);

    if (!GetCommState(m_handle, &dcb)) {
        close();
        return false;
    }

    dcb.BaudRate = baudRate;
    dcb.ByteSize = static_cast<BYTE>(dataBits);
    // dcb.StopBits = static_cast<BYTE>(stopBits);
    switch (stopBits) {
        case StopBits::One: dcb.StopBits = ONESTOPBIT; break;
        case StopBits::OnePointFive: dcb.StopBits = ONE5STOPBITS; break;
        case StopBits::Two: dcb.StopBits = TWOSTOPBITS; break;
    }
//     BYTE winStopBits = 0; // default ONESTOPBIT
// if (stopBits == 1) winStopBits = ONESTOPBIT;
// else if (stopBits == 1.5) winStopBits = ONE5STOPBITS;
// else if (stopBits == 2) winStopBits = TWOSTOPBITS;
// dcb.StopBits = winStopBits;
    applyFlowControl(dcb, flowControl);

    dcb.Parity = static_cast<BYTE>(parity);

    dcb.fBinary = TRUE;
    dcb.fParity = (parity != SerialPort::Parity::None);
    // dcb.fOutxCtsFlow = FALSE;
    dcb.fOutxDsrFlow = FALSE;
    dcb.fDtrControl = DTR_CONTROL_ENABLE;
    dcb.fDsrSensitivity = FALSE;
    dcb.fTXContinueOnXoff = TRUE;
    // dcb.fOutX = FALSE;
    // dcb.fInX = FALSE;
    dcb.fErrorChar = FALSE;
    dcb.fRtsControl = RTS_CONTROL_ENABLE;
    dcb.fAbortOnError = FALSE;

    std::ostringstream logStream;
    logStream << "Configuring COM port " << m_portName << " with settings:\n";
    logStream << "  BaudRate: " << dcb.BaudRate << "\n";
    logStream << "  ByteSize: " << static_cast<int>(dcb.ByteSize) << "\n";
    logStream << "  StopBits: " << static_cast<int>(dcb.StopBits) << "\n";
    logStream << "  Parity: " << static_cast<int>(dcb.Parity) << "\n";
    logStream << "  fOutxCtsFlow: " << dcb.fOutxCtsFlow << "\n";
    logStream << "  fOutX: " << dcb.fOutX << "\n";
    logStream << "  fInX: " << dcb.fInX << "\n";
    logStream << "  fDtrControl: " << dcb.fDtrControl << "\n";
    logStream << "  fRtsControl: " << dcb.fRtsControl << "\n";

    std::string logStr = logStream.str();
    OutputDebugStringA(logStr.c_str());

    if (!SetCommState(m_handle, &dcb)) {
        DWORD err = GetLastError();
        OutputDebugStringA(("SetCommState failed. Error code: " + std::to_string(err) + "\n").c_str());
        close();
        return false;
    }

    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = MAXDWORD;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = 0;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 0;

    if (!SetCommTimeouts(m_handle, &timeouts)) {
        close();
        return false;
    }

    m_isRunning = true;
    startReading();
    return true;
}

void SerialPort::close() {
    stopReading();
    if (m_handle != INVALID_HANDLE_VALUE) {
        CloseHandle(m_handle);
        m_handle = INVALID_HANDLE_VALUE;
    }
}

bool SerialPort::write(const std::vector<uint8_t>& data) {
    if (!isOpen()) {
        OutputDebugStringA("SerialPort::write - port not open\n");
        return false;
    }
    if (data.empty()) {
        OutputDebugStringA("SerialPort::write - data empty\n");
        return false;
    }

    DWORD bytesWritten;
    BOOL result = WriteFile(m_handle, data.data(), static_cast<DWORD>(data.size()), &bytesWritten, nullptr);
    if (!result) {
        DWORD err = GetLastError();
        OutputDebugStringA(("SerialPort::write - WriteFile failed with error code: " + std::to_string(err) + "\n").c_str());
        return false;
    }
    OutputDebugStringA(("SerialPort::write - WriteFile succeeded, bytes written: " + std::to_string(bytesWritten) + "\n").c_str());
    return true;
}

void SerialPort::setDataReceivedCallback(DataReceivedCallback callback) {
    m_dataCallback = std::move(callback);
}

void SerialPort::startReading() {
    if (!m_isReading) {
        m_isReading = true;
        m_readThread = std::thread(&SerialPort::readThread, this);
    }
}

void SerialPort::readThread() {
    std::vector<uint8_t> buffer(1024);
    DWORD bytesRead;
    OVERLAPPED overlapped = { 0 };
    overlapped.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);

    while (m_isRunning) {
        if (!ReadFile(m_handle, buffer.data(), buffer.size(), &bytesRead, &overlapped)) {
            if (GetLastError() != ERROR_IO_PENDING) {
                break;
            }

            if (WaitForSingleObject(overlapped.hEvent, 100) == WAIT_OBJECT_0) {
                if (GetOverlappedResult(m_handle, &overlapped, &bytesRead, FALSE)) {
                    if (bytesRead > 0 && m_dataCallback) {
                        OutputDebugStringA(("Received " + std::to_string(bytesRead) + " bytes\n").c_str());
                        m_dataCallback(std::vector<uint8_t>(buffer.begin(), buffer.begin() + bytesRead));
                    }
                }
            }
        } else if (bytesRead > 0 && m_dataCallback) {
            OutputDebugStringA(("Received " + std::to_string(bytesRead) + " bytes\n").c_str());
            m_dataCallback(std::vector<uint8_t>(buffer.begin(), buffer.begin() + bytesRead));
        }
        ResetEvent(overlapped.hEvent);
    }

    CloseHandle(overlapped.hEvent);
}

void SerialPort::stopReading() {
    m_isRunning = false;
    if (m_readThread.joinable()) {
        m_readThread.join();
    }
    m_isReading = false;
}