import { NativeEventEmitter } from 'react-native';
import SerialportWindows from './NativeSerialportWindows';

export const eventEmitter = new NativeEventEmitter(SerialportWindows);

export function listPorts(): Promise<string[]> {
  return SerialportWindows.listPorts();
}

export function openPort(
  portName: string,
  baudRate: number,
  dataBits: number,
  stopBits: number,
  parity: number,
  flowControl: number
): Promise<string> {
  return SerialportWindows.openPort(
    portName,
    baudRate,
    dataBits,
    stopBits,
    parity,
    flowControl
  );
}

export function closePort(): Promise<string> {
  return SerialportWindows.closePort();
}

export function write(data: number[]): Promise<boolean> {
  return SerialportWindows.write(data);
}
