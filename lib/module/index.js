"use strict";

import { NativeEventEmitter } from 'react-native';
import SerialportWindows from "./NativeSerialportWindows.js";
export const eventEmitter = new NativeEventEmitter(SerialportWindows);
export function listPorts() {
  return SerialportWindows.listPorts();
}
export function openPort(portName, baudRate, dataBits, stopBits, parity, flowControl) {
  return SerialportWindows.openPort(portName, baudRate, dataBits, stopBits, parity, flowControl);
}
export function closePort(portName) {
  return SerialportWindows.closePort(portName);
}
export function write(portName, data) {
  return SerialportWindows.write(portName, data);
}
//# sourceMappingURL=index.js.map