#pragma once

#include "ReactPackageProvider.g.h"

using namespace winrt::Microsoft::ReactNative;

namespace winrt::ReactNativeSerialportWindows::implementation
{

struct ReactPackageProvider : ReactPackageProviderT<ReactPackageProvider>
{
  ReactPackageProvider() = default;

  void CreatePackage(IReactPackageBuilder const &packageBuilder) noexcept;
};

} // namespace winrt::ReactNativeSerialportWindows::implementation

namespace winrt::ReactNativeSerialportWindows::factory_implementation
{

struct ReactPackageProvider : ReactPackageProviderT<ReactPackageProvider, implementation::ReactPackageProvider> {};

} // namespace winrt::ReactNativeSerialportWindows::factory_implementation
