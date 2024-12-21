import type { TurboModule } from 'react-native';
import { TurboModuleRegistry } from 'react-native';

export interface Spec extends TurboModule {
  multiply(a: number, b: number): number;
  listPorts(): Promise<string[]>;
  /*openPort(
    portName: string,
    baudRate: number,
    dataBits: number,
    stopBits: number,
    parity: number,
    handshake: number
  ): Promise<string>;
  onDataReceived(callback: (data: string) => void): void;*/
}

export default TurboModuleRegistry.getEnforcing<Spec>(
  'ReactNativeSerialportWindows'
);
