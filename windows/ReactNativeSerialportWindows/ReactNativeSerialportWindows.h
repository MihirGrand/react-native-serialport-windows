#pragma once

#include "pch.h"
#include "resource.h"
#include "SerialPort.h"

#if __has_include("codegen/NativeSerialportWindowsDataTypes.g.h")
  #include "codegen/NativeSerialportWindowsDataTypes.g.h"
#endif
#include "codegen/NativeSerialportWindowsSpec.g.h"

#include "NativeModules.h"

namespace winrt::ReactNativeSerialportWindows
{

REACT_MODULE(ReactNativeSerialportWindows)
struct ReactNativeSerialportWindows
{
  using ModuleSpec = ReactNativeSerialportWindowsCodegen::SerialportWindowsSpec;

  REACT_INIT(Initialize)
  void Initialize(React::ReactContext const &reactContext) noexcept;

  REACT_METHOD(listPorts)
  void listPorts(React::ReactPromise<std::vector<std::string>> promise) noexcept;

  REACT_METHOD(openPort)
  void openPort(std::string portName, double baudRate, double dataBits, 
                double stopBits, double parity, double flowControl,
                React::ReactPromise<std::string> &&promise) noexcept;

  REACT_METHOD(closePort)
  void closePort(React::ReactPromise<std::string> &&promise) noexcept;

  REACT_METHOD(write)
  void write(std::vector<double> const& data, React::ReactPromise<bool> &&promise) noexcept;

  REACT_METHOD(addListener)
  void addListener(std::string eventType) noexcept;

  REACT_METHOD(removeListeners)
  void removeListeners(double count) noexcept;

  void OnDataReceived(const std::vector<uint8_t>& data);

private:
  React::ReactContext m_context;
  std::unique_ptr<SerialPort> m_serialPort;
};

} // namespace winrt::ReactNativeSerialportWindows
