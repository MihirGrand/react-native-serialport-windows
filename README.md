# [react-native-serialport-windows](https://www.npmjs.com/package/react-native-serialport-windows)

Serial Communication for React Native Windows apps

## Installation

```sh
npm install react-native-serialport-windows
```

```js
import {
  listPorts,
  openPort,
  closePort,
  write,
  eventEmitter,
} from 'react-native-serialport-windows';
```

## Usage

### 1. List available ports

#### Retrieve a list of all available serial ports:

```js
const availablePorts = await listPorts();
```

### 1. Open a port

#### Open a port with the desired settings:

```js
await openPort('COM1', 9600, 8, 1, 0, 0);
// Parameters: portName, baudRate, dataBits, stopBits, parity, flowControl
```

### 2. Write data

#### Send data to the serial port. Data should be an array of byte values:

```js
const data = [0x48, 0x65, 0x6c, 0x6c, 0x6f]; // "Hello"
await write(data);
```

### 3. Receive data

#### Listen for incoming data using the eventEmitter:

```js
useEffect(() => {
  const subscription = eventEmitter.addListener(
    'SerialPortDataReceived',
    ({ data }) => {
      const text = String.fromCharCode(...data);
      console.log('Received:', text);
    }
  );

  return () => subscription.remove();
}, []);
```

### 4. Close the port

#### When finished, ensure the port is closed:

```js
await closePort();
```

## Example

```sh
git clone https://github.com/MihirGrand/react-native-serialport-windows.git

cd react-native-serialport-windows

npm install

cd example

npm run windows
```

## Contributing

```sh
git checkout -b my-feature-branch

git add .
git commit -m "feat: New feature description"

git push origin my-feature-branch
```

and create a PR!

## License

MIT
