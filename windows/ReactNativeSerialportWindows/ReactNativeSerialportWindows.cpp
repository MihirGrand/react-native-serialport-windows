#include "pch.h"
#include "ReactNativeSerialportWindows.h"
#include <SetupAPI.h>
#include <initguid.h>
#include <devguid.h>
#include "SerialPort.h"
#pragma comment(lib, "setupapi.lib")

namespace winrt::ReactNativeSerialportWindows {

    void ReactNativeSerialportWindows::Initialize(React::ReactContext const& reactContext) noexcept {
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
                    }
                    else {
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
        React::ReactPromise<std::string>&& promise) noexcept {
        try {
            OutputDebugStringA(("Opening port: " + portName + "\n").c_str());

            if (portName.find("\\\\.\\") != 0) {
                portName = "\\\\.\\" + portName;
            }

            auto serialPort = std::make_unique<SerialPort>(portName);

            int winStopBits = static_cast<int>(stopBits);
            int winParity = static_cast<int>(parity);

            bool success = serialPort->open(
                static_cast<int>(baudRate),
                static_cast<SerialPort::DataBits>(static_cast<int>(dataBits)),
                static_cast<SerialPort::StopBits>(winStopBits),
                static_cast<SerialPort::Parity>(winParity),
                static_cast<SerialPort::FlowControl>(static_cast<int>(flowControl))
            );

            if (success) {
                serialPort->setDataReceivedCallback([this, portName](const std::vector<uint8_t>& data) {
                    OnDataReceived(portName, data);
                    });
                m_serialPorts[portName] = std::move(serialPort);
                promise.Resolve("Port opened successfully");
            }
            else {
                DWORD error = GetLastError();
                char errorMsg[256];
                FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, error, 0, errorMsg, sizeof(errorMsg), nullptr);
                promise.Reject(React::ReactError{ "Error", "Failed to open port. Error " + std::to_string(error) + ": " + errorMsg });
            }
        }
        catch (const std::exception& e) {
            promise.Reject(React::ReactError{ "Error", e.what() });
        }
    }

    void ReactNativeSerialportWindows::closePort(std::string portName, React::ReactPromise<std::string>&& promise) noexcept {
        // Ensure portName format matches what's stored in m_serialPorts
        if (portName.find("\\\\.\\") != 0) {
            portName = "\\\\.\\" + portName;
        }
        auto it = m_serialPorts.find(portName);
        if (it != m_serialPorts.end()) {
            it->second->close();
            m_serialPorts.erase(it);
            promise.Resolve("Port closed successfully");
        }
        else {
            promise.Reject(React::ReactError{ "Error", "Port not open" });
        }
    }

    void ReactNativeSerialportWindows::write(std::string portName, std::vector<double> const& data,
        React::ReactPromise<bool>&& promise) noexcept {
        // Ensure portName format matches what's stored in m_serialPorts
        if (portName.find("\\\\.\\") != 0) {
            portName = "\\\\.\\" + portName;
        }
        OutputDebugStringA(("Attempting to write to port: " + portName + "\n").c_str());

        auto it = m_serialPorts.find(portName);
        if (it == m_serialPorts.end()) {
            OutputDebugStringA(("Port not open: " + portName + "\n").c_str());
            promise.Reject("Port not open");
            return;
        }

        std::vector<uint8_t> byteData;
        byteData.reserve(data.size());
        for (const auto& value : data) {
            byteData.push_back(static_cast<uint8_t>(value));
        }

        bool writeResult = it->second->write(byteData);
        if (writeResult) {
            OutputDebugStringA(("WriteFile succeeded, bytes written: " + std::to_string(byteData.size()) + "\n").c_str());
            promise.Resolve(true);
        } else {
            // Get the last error code from Windows
            DWORD err = GetLastError();
            OutputDebugStringA(("WriteFile failed on port: " + portName + ", error code: " + std::to_string(err) + "\n").c_str());
            promise.Resolve(false); // or promise.Reject("Write failed");
        }
    }

    void ReactNativeSerialportWindows::OnDataReceived(const std::string& portName, const std::vector<uint8_t>& data) {
        if (m_context) {
            auto eventData = winrt::Microsoft::ReactNative::JSValueObject();
            winrt::Microsoft::ReactNative::JSValueArray jsDataArray;
            for (auto byte : data) {
                jsDataArray.push_back(static_cast<int>(byte));
            }

            eventData["port"] = portName;
            eventData["data"] = std::move(jsDataArray);

            m_context.EmitJSEvent(L"RCTDeviceEventEmitter", L"SerialPortDataReceived", eventData);
        }
    }

    void ReactNativeSerialportWindows::addListener(std::string eventName) noexcept {
        // No-op
    }

    void ReactNativeSerialportWindows::removeListeners(double count) noexcept {
        // No-op
    }

} // namespace winrt::ReactNativeSerialportWindows
