"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.closePort = closePort;
exports.eventEmitter = void 0;
exports.listPorts = listPorts;
exports.openPort = openPort;
exports.write = write;
var _reactNative = require("react-native");
var _NativeSerialportWindows = _interopRequireDefault(require("./NativeSerialportWindows.js"));
function _interopRequireDefault(e) { return e && e.__esModule ? e : { default: e }; }
const eventEmitter = exports.eventEmitter = new _reactNative.NativeEventEmitter(_NativeSerialportWindows.default);
function listPorts() {
  return _NativeSerialportWindows.default.listPorts();
}
function openPort(portName, baudRate, dataBits, stopBits, parity, flowControl) {
  return _NativeSerialportWindows.default.openPort(portName, baudRate, dataBits, stopBits, parity, flowControl);
}
function closePort(portName) {
  return _NativeSerialportWindows.default.closePort(portName);
}
function write(portName, data) {
  return _NativeSerialportWindows.default.write(portName, data);
}
//# sourceMappingURL=index.js.map