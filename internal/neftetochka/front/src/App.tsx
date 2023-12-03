import LoginPage from './pages/Login';
import HomePage from './pages/Home';
import * as api from './api';



function App() {
  const { isError, data } = api.useStations();
  
  if (data && !isError) {
    return <HomePage stations={data} />
  }

  return <LoginPage />
}

export default App;
