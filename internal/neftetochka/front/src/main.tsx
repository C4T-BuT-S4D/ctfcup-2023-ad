import React from 'react'
import ReactDOM from 'react-dom/client'
import { HTTPError } from "ky";
import { QueryClient, QueryClientProvider } from "@tanstack/react-query";

import App from './App.tsx'
import './index.css'


const queryClient = new QueryClient();
queryClient.setDefaultOptions({
  queries: {
    retry(_, error) {
      if (error instanceof HTTPError && error.response.status == 401) {
        return false;
      }

      return true;
    },
  },
});

ReactDOM.createRoot(document.getElementById('root')!).render(
  <React.StrictMode>
    <QueryClientProvider client={queryClient}>
      <App />
    </QueryClientProvider>
  </React.StrictMode>,
)
