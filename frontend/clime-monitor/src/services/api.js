import axios from 'axios';

const API_BASE = import.meta.env.VITE_API_URL || '/api';

const api = axios.create({
  baseURL: API_BASE,
  headers: {
    'Content-Type': 'application/json',
  },
});

export const getLecturas = () => api.get('/lecturas/');
export const getLatestLectura = () => api.get('/lecturas/latest/');

export default api;
