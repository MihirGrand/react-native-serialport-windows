import { Text, View, StyleSheet } from 'react-native';
import { /*multiply,*/ listPorts } from 'react-native-serialport-windows';
import { useState, useEffect } from 'react';

//const result = multiply(3, 7);

export default function App() {
  const [ports, setPorts] = useState<string[]>([]);
  const [error, setError] = useState<string>('');

  useEffect(() => {
    const getPorts = async () => {
      try {
        const availablePorts = await listPorts();
        setPorts(availablePorts);
      } catch (err) {
        setError(err instanceof Error ? err.message : 'Failed to list ports');
      }
    };
    getPorts();
  }, []);

  return (
    <View style={styles.container}>
      <Text style={styles.header}>Available Ports:</Text>
      {error ? (
        <Text style={styles.error}>{error}</Text>
      ) : (
        ports.map((port, index) => (
          <Text key={index} style={styles.port}>
            {port}
          </Text>
        ))
      )}
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    alignItems: 'center',
    justifyContent: 'center',
  },
  header: {
    marginTop: 20,
    fontSize: 16,
    fontWeight: 'bold',
  },
  port: {
    marginTop: 5,
    fontSize: 14,
  },
  error: {
    color: 'red',
    marginTop: 10,
  },
});
