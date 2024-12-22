
/*
 * This file is auto-generated from a NativeModule spec file in js.
 *
 * This is a C++ Spec class that should be used with MakeTurboModuleProvider to register native modules
 * in a way that also verifies at compile time that the native module matches the interface required
 * by the TurboModule JS spec.
 */
#pragma once
// clang-format off


#include <NativeModules.h>
#include <tuple>

namespace ReactNativeSerialportWindowsCodegen {

struct SerialportWindowsSpec : winrt::Microsoft::ReactNative::TurboModuleSpec {
  static constexpr auto methods = std::tuple{
      Method<void(Promise<std::vector<std::string>>) noexcept>{0, L"listPorts"},
      Method<void(std::string, double, double, double, double, double, Promise<std::string>) noexcept>{1, L"openPort"},
      Method<void(Promise<std::string>) noexcept>{2, L"closePort"},
      Method<void(std::vector<double>, Promise<bool>) noexcept>{3, L"write"},
      Method<void(std::string) noexcept>{4, L"addListener"},
      Method<void(double) noexcept>{5, L"removeListeners"},
  };

  template <class TModule>
  static constexpr void ValidateModule() noexcept {
    constexpr auto methodCheckResults = CheckMethods<TModule, SerialportWindowsSpec>();

    REACT_SHOW_METHOD_SPEC_ERRORS(
          0,
          "listPorts",
          "    REACT_METHOD(listPorts) void listPorts(::React::ReactPromise<std::vector<std::string>> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(listPorts) static void listPorts(::React::ReactPromise<std::vector<std::string>> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          1,
          "openPort",
          "    REACT_METHOD(openPort) void openPort(std::string portName, double baudRate, double dataBits, double stopBits, double parity, double flowControl, ::React::ReactPromise<std::string> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(openPort) static void openPort(std::string portName, double baudRate, double dataBits, double stopBits, double parity, double flowControl, ::React::ReactPromise<std::string> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          2,
          "closePort",
          "    REACT_METHOD(closePort) void closePort(::React::ReactPromise<std::string> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(closePort) static void closePort(::React::ReactPromise<std::string> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          3,
          "write",
          "    REACT_METHOD(write) void write(std::vector<double> const & data, ::React::ReactPromise<bool> &&result) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(write) static void write(std::vector<double> const & data, ::React::ReactPromise<bool> &&result) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          4,
          "addListener",
          "    REACT_METHOD(addListener) void addListener(std::string eventType) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(addListener) static void addListener(std::string eventType) noexcept { /* implementation */ }\n");
    REACT_SHOW_METHOD_SPEC_ERRORS(
          5,
          "removeListeners",
          "    REACT_METHOD(removeListeners) void removeListeners(double count) noexcept { /* implementation */ }\n"
          "    REACT_METHOD(removeListeners) static void removeListeners(double count) noexcept { /* implementation */ }\n");
  }
};

} // namespace ReactNativeSerialportWindowsCodegen
