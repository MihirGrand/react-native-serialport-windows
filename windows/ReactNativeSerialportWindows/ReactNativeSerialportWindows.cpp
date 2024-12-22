#include "pch.h"
#include "ReactNativeSerialportWindows.h"
#include <SetupAPI.h>
#include <initguid.h>
#include <devguid.h>
#include "SerialPort.h"
#pragma comment(lib, "setupapi.lib")

namespace winrt::ReactNativeSerialportWindows
{

void ReactNativeSerialportWindows::Initialize(React::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
}

void ReactNativeSerialportWindows::listPorts(React::ReactPromise<std::vector<std::string>> promise) noexcept {
    std::vector<std::string> portList;
    
    HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_PORTS, nullptr, nullptr, DIGCF_PRESENT);
    if (hDevInfo == INVALID_HANDLE_VALUE) {
        promise.Reject(React::ReactError{ "Error", "Failed to get device information set for ports." });
        return;
    }

    SP_DEVINFO_DATA devInfo;
    devInfo.cbSize = sizeof(SP_DEVINFO_DATA);

    for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &devInfo); i++) {
        HKEY hKey = SetupDiOpenDevRegKey(hDevInfo, &devInfo, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
        if (hKey) {
            TCHAR portName[256];
            DWORD size = sizeof(portName);
            DWORD type = 0;
            if (RegQueryValueEx(hKey, TEXT("PortName"), nullptr, &type, (LPBYTE)portName, &size) == ERROR_SUCCESS) {
                std::string portString;
                if constexpr (sizeof(TCHAR) == sizeof(wchar_t)) {
                    int len = WideCharToMultiByte(CP_UTF8, 0, portName, -1, nullptr, 0, nullptr, nullptr);
                    if (len > 0) {
                        portString.resize(len - 1);
                        WideCharToMultiByte(CP_UTF8, 0, portName, -1, &portString[0], len, nullptr, nullptr);
                    }
                } else {
                    portString = std::string(portName, portName + size / sizeof(TCHAR) - 1);
                }
                portList.push_back(portString);
            }
            RegCloseKey(hKey);
        }
    }

    SetupDiDestroyDeviceInfoList(hDevInfo);
    promise.Resolve(portList);
}

void ReactNativeSerialportWindows::openPort(std::string portName, double baudRate, 
    double dataBits, double stopBits, double parity, double flowControl,
    React::ReactPromise<std::string> &&promise) noexcept {
    try {
        OutputDebugStringA(("Opening port with params:\n"
            "Port: " + portName + "\n"
            "Baud: " + std::to_string(static_cast<int>(baudRate)) + "\n"
            "Data: " + std::to_string(static_cast<int>(dataBits)) + "\n"
            "Stop: " + std::to_string(static_cast<int>(stopBits)) + "\n"
            "Parity: " + std::to_string(static_cast<int>(parity)) + "\n").c_str());

        if (portName.find("\\\\.\\") != 0) {
            portName = "\\\\.\\" + portName;
        }

        m_serialPort = std::make_unique<SerialPort>(portName);
        
        int winStopBits;
        switch(static_cast<int>(stopBits)) {
            case 1: winStopBits = ONESTOPBIT; break;
            case 2: winStopBits = TWOSTOPBITS; break;
            default: winStopBits = ONESTOPBIT;
        }

        int winParity;
        switch(static_cast<int>(parity)) {
            case 0: winParity = NOPARITY; break;
            case 1: winParity = ODDPARITY; break;
            case 2: winParity = EVENPARITY; break;
            default: winParity = NOPARITY;
        }

        bool success = m_serialPort->open(
            static_cast<int>(baudRate),
            static_cast<SerialPort::DataBits>(dataBits),
            static_cast<SerialPort::StopBits>(winStopBits),
            static_cast<SerialPort::Parity>(winParity),
            static_cast<SerialPort::FlowControl>(flowControl)
        );

        if (success) {
            m_serialPort->setDataReceivedCallback([this](const std::vector<uint8_t>& data) {
                OnDataReceived(data);
            });
            promise.Resolve("Port opened successfully");
        } else {
            DWORD error = GetLastError();
            char errorMsg[256];
            FormatMessageA(
                FORMAT_MESSAGE_FROM_SYSTEM,
                nullptr,
                error,
                0,
                errorMsg,
                sizeof(errorMsg),
                nullptr
            );
            promise.Reject(React::ReactError{"Error", 
                std::string("Failed to open port. Error ") + 
                std::to_string(error) + ": " + errorMsg});
        }
    } catch (const std::exception& e) {
        promise.Reject(React::ReactError{"Error", e.what()});
    }
}

void ReactNativeSerialportWindows::closePort(React::ReactPromise<std::string> &&promise) noexcept {
    if (m_serialPort) {
        m_serialPort->close();
        m_serialPort.reset();
        promise.Resolve("Port closed successfully");
    } else {
        promise.Reject(React::ReactError{ "Error", "No port open" });
    }
}

void ReactNativeSerialportWindows::write(std::vector<double> const& data, 
    React::ReactPromise<bool> &&promise) noexcept {
    if (!m_serialPort) {
        promise.Reject("No port open");
        return;
    }

    std::vector<uint8_t> byteData;
    byteData.reserve(data.size());
    for (const auto& value : data) {
        byteData.push_back(static_cast<uint8_t>(value));
    }

    promise.Resolve(m_serialPort->write(byteData));
}

void ReactNativeSerialportWindows::OnDataReceived(const std::vector<uint8_t>& data) {
    if (m_context) {
        auto eventData = winrt::Microsoft::ReactNative::JSValueObject();
        winrt::Microsoft::ReactNative::JSValueArray jsDataArray;
        for (auto byte : data) {
            jsDataArray.push_back(static_cast<int>(byte));
        }

        eventData["data"] = std::move(jsDataArray);

        m_context.EmitJSEvent(L"RCTDeviceEventEmitter", L"SerialPortDataReceived", eventData);
    }
}

void ReactNativeSerialportWindows::addListener(std::string eventType) noexcept {
    // 
}

void ReactNativeSerialportWindows::removeListeners(double count) noexcept {
    //
}

} // namespace winrt::ReactNativeSerialportWindows
