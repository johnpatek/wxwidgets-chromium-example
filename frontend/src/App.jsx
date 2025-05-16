import { useState } from 'react'
import './App.css'

function App() {
  const [text, setText] = useState('');

  const handleSend = async () => {
    try {
      const response = await fetch('https://wxreact.ipc/EchoMessage', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
        },
        body: JSON.stringify({ message: text }),
      });

      const result = await response.json();
      console.log('Server response:', result);
    } catch (err) {
      console.error('POST request failed:', err);
    }
  };

  return (
    <>
      <h1>Vite + React</h1>
      <div className="card">
        <input
          type="text"
          value={text}
          onChange={(e) => setText(e.target.value)}
          placeholder="Enter message"
        />
        <button onClick={handleSend}>Send</button>
      </div>
    </>
  );
}

export default App
