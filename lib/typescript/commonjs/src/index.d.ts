import { NativeEventEmitter } from 'react-native';
export declare const eventEmitter: NativeEventEmitter;
export declare function listPorts(): Promise<string[]>;
export declare function openPort(portName: string, baudRate: number, dataBits: number, stopBits: number, parity: number, flowControl: number): Promise<string>;
export declare function closePort(portName: String): Promise<string>;
export declare function write(portName: String, data: number[]): Promise<boolean>;
//# sourceMappingURL=index.d.ts.map