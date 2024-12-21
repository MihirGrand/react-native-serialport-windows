import SerialportWindows from './NativeSerialportWindows';

export function multiply(a: number, b: number): number {
  return SerialportWindows.multiply(a, b);
}

export function listPorts(): Promise<string[]> {
  return SerialportWindows.listPorts();
}

/*export function openPort(
  portName: string,
  baudRate: number,
  dataBits: number,
  stopBits: number,
  parity: number,
  handshake: number
): Promise<string> {
  return SerialportWindows.openPort(
    portName,
    baudRate,
    dataBits,
    stopBits,
    parity,
    handshake
  );
}

export function onDataReceived(callback: (data: string) => void): void {
  SerialportWindows.onDataReceived(callback);
}*/
