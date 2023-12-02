import { HTTPError } from "ky";

import LoginPage from './pages/Login';
import HomePage from './pages/Home';
import * as api from './api';



function App() {
  const { isLoading, isError, data, error } = api.useStations();

  const authError = error instanceof HTTPError && (error.response.status == 401);
  
  if (data && !isError) {
    return <HomePage stations={data} />
  }
  console.log(error);

  if (!isLoading && isError) {
    if (!authError) {
      console.log(error);
    }
  }
  return <LoginPage />
}

export default App;
