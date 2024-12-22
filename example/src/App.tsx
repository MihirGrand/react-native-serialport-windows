import { Text, View, StyleSheet, Button } from 'react-native';
import {
  listPorts,
  openPort,
  eventEmitter,
} from 'react-native-serialport-windows';
import { useState, useEffect } from 'react';

const byteArrayToString = (data: number[]): string => {
  return String.fromCharCode(...data);
};

export default function App() {
  const [ports, setPorts] = useState<string[]>([]);
  const [error, setError] = useState<string>('');
  const [selectedPort, setSelectedPort] = useState<string>('');
  const [receivedData, setReceivedData] = useState<string>('');

  useEffect(() => {
    const getPorts = async () => {
      try {
        const availablePorts = await listPorts();
        setPorts(availablePorts);
        if (availablePorts.length > 0) {
          setSelectedPort(availablePorts[0] || '');
        }
      } catch (err) {
        setError(err instanceof Error ? err.message : 'Failed to list ports');
      }
    };
    getPorts();
  }, []);

  useEffect(() => {
    const subscription = eventEmitter.addListener(
      'SerialPortDataReceived',
      ({ data }) => {
        const text = byteArrayToString(data);
        setReceivedData((prevData) => prevData + text);
      }
    );

    return () => subscription.remove();
  }, []);

  const handleOpenPort = async () => {
    if (!selectedPort) return;

    try {
      await openPort(selectedPort, 9600, 8, 1, 0, 0);
      console.log('Port opened successfully');
    } catch (err) {
      console.error('Error opening port:', err);
      setError(err instanceof Error ? err.message : 'Failed to open port');
    }
  };

  return (
    <View style={styles.container}>
      <Text style={styles.header}>Available Ports:</Text>
      {error ? (
        <Text style={styles.error}>{error}</Text>
      ) : (
        <>
          {ports.map((port, index) => (
            <Text key={index} style={styles.port}>
              {port}
            </Text>
          ))}
          {selectedPort && (
            <Button title={`Open ${selectedPort}`} onPress={handleOpenPort} />
          )}
          {receivedData && (
            <View style={styles.dataContainer}>
              <Text style={styles.header}>Received Data:</Text>
              <Text style={styles.data}>{receivedData}</Text>
            </View>
          )}
        </>
      )}
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    alignItems: 'center',
    justifyContent: 'center',
    padding: 20,
  },
  header: {
    fontSize: 16,
    fontWeight: 'bold',
    marginBottom: 10,
  },
  port: {
    marginTop: 5,
    fontSize: 14,
  },
  error: {
    color: 'red',
    marginTop: 10,
  },
  dataContainer: {
    marginTop: 20,
    padding: 10,
    backgroundColor: '#f0f0f0',
    borderRadius: 5,
    width: '100%',
  },
  data: {
    fontFamily: 'monospace',
    fontSize: 12,
  },
});
