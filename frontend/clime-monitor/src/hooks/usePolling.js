import { useState, useEffect, useCallback, useRef } from 'react';

export function usePolling(fetchFn, interval = 10000) {
  const [data, setData] = useState(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);
  const intervalRef = useRef(null);

  const fetchData = useCallback(async () => {
    try {
      const response = await fetchFn();
      setData(response.data);
      setError(null);
    } catch (err) {
      setError(err.message || 'Error al obtener datos');
    } finally {
      setLoading(false);
    }
  }, [fetchFn]);

  useEffect(() => {
    fetchData();
    intervalRef.current = setInterval(fetchData, interval);
    return () => {
      if (intervalRef.current) {
        clearInterval(intervalRef.current);
      }
    };
  }, [fetchData, interval]);

  return { data, loading, error };
}
