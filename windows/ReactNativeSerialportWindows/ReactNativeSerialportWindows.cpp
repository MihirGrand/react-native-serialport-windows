#include "pch.h"
#include "ReactNativeSerialportWindows.h"
#include <SetupAPI.h>
#include <initguid.h>
#include <devguid.h>
#pragma comment(lib, "setupapi.lib")

namespace winrt::ReactNativeSerialportWindows
{

void ReactNativeSerialportWindows::Initialize(React::ReactContext const &reactContext) noexcept {
  m_context = reactContext;
}

double ReactNativeSerialportWindows::multiply(double a, double b) noexcept {
  return a * b;
}

void ReactNativeSerialportWindows::listPorts(React::ReactPromise<std::vector<std::string>> promise) noexcept {
    std::vector<std::string> portList;
    
    HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_PORTS, nullptr, nullptr, DIGCF_PRESENT);
    if (hDevInfo == INVALID_HANDLE_VALUE) {
        promise.Reject("Failed to get device information set for ports.");
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

/*void ReactNativeSerialportWindows::openPort(std::string portName, double baudRate, double dataBits, double stopBits, double parity, double handshake, React::ReactPromise<std::string> promise) noexcept {
  if (m_serialPortHandle != INVALID_HANDLE_VALUE) {
    promise.Reject("Port already open.");
    return;
  }

  m_serialPortHandle = CreateFileA(
      portName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
  
  if (m_serialPortHandle == INVALID_HANDLE_VALUE) {
    promise.Reject("Failed to open serial port.");
    return;
  }

  DCB dcbSerialParams = {0};
  dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

  if (!GetCommState(m_serialPortHandle, &dcbSerialParams)) {
    promise.Reject("Failed to get current serial port state.");
    return;
  }

  dcbSerialParams.BaudRate = static_cast<DWORD>(baudRate); // Ensure correct casting
  dcbSerialParams.ByteSize = static_cast<BYTE>(dataBits);  // Ensure correct casting
  dcbSerialParams.StopBits = (stopBits == 1) ? ONESTOPBIT : TWOSTOPBITS;
  dcbSerialParams.Parity = (parity == 0) ? NOPARITY : (parity == 1) ? ODDPARITY : EVENPARITY;
  dcbSerialParams.fOutxCtsFlow = handshake == 1;
  dcbSerialParams.fRtsControl = RTS_CONTROL_HANDSHAKE;

  if (!SetCommState(m_serialPortHandle, &dcbSerialParams)) {
    promise.Reject("Failed to set serial port configuration.");
    return;
  }

  m_isReading = true;
  m_readThread = std::thread(&ReactNativeSerialportWindows::startReading, this);

  promise.Resolve("Port opened successfully.");
}

void ReactNativeSerialportWindows::startReading() {
  while (m_isReading) {
    DWORD bytesRead;
    char buffer[1024] = {0};

    if (ReadFile(m_serialPortHandle, buffer, sizeof(buffer), &bytesRead, nullptr)) {
      if (bytesRead > 0) {
        emitDataReceived(std::string(buffer, bytesRead));
      }
    } else {
      break;
    }
  }
}

void ReactNativeSerialportWindows::stopReading() {
  m_isReading = false;
  if (m_readThread.joinable()) {
    m_readThread.join();
  }
}

void ReactNativeSerialportWindows::onDataReceived(std::function<void(std::string)> const & callback) noexcept {
    m_dataCallback = callback;
}

void ReactNativeSerialportWindows::emitDataReceived(std::string data) {
    if (m_dataCallback) {
        m_dataCallback(data);
    }
}*/

} // namespace winrt::ReactNativeSerialportWindows
