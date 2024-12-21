#pragma once

#include "pch.h"
#include "resource.h"

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

  REACT_SYNC_METHOD(multiply)
  double multiply(double a, double b) noexcept;

  REACT_METHOD(listPorts)
  void listPorts(React::ReactPromise<std::vector<std::string>> promise) noexcept;

  /*REACT_METHOD(openPort)
  void openPort(std::string portName, double baudRate, double dataBits, double stopBits, double parity, double handshake, React::ReactPromise<std::string> promise) noexcept;

  REACT_METHOD(onDataReceived)
  void onDataReceived(std::function<void(std::string)> const & callback) noexcept;


  void emitDataReceived(std::string data);*/

private:
  React::ReactContext m_context;
  /*HANDLE m_serialPortHandle = INVALID_HANDLE_VALUE;
  bool m_isReading = false;
  std::thread m_readThread;
  std::function<void(std::string)> m_dataCallback;
  
  void startReading();
  void stopReading();*/
};

} // namespace winrt::ReactNativeSerialportWindows
