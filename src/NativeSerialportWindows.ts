import type { TurboModule } from 'react-native';
import { TurboModuleRegistry } from 'react-native';

export interface Spec extends TurboModule {
  listPorts(): Promise<string[]>;
  openPort(
    portName: string,
    baudRate: number,
    dataBits: number,
    stopBits: number,
    parity: number,
    flowControl: number
  ): Promise<string>;
  closePort(): Promise<string>;
  write(data: Array<number>): Promise<boolean>;
  addListener(eventType: string): void;
  removeListeners(count: number): void;
}

export default TurboModuleRegistry.getEnforcing<Spec>(
  'ReactNativeSerialportWindows'
);
