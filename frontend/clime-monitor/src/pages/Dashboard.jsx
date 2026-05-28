import { getLecturas, getLatestLectura } from '../services/api';
import { usePolling } from '../hooks/usePolling';
import SensorCard from '../components/SensorCard';
import SensorChart from '../components/SensorChart';
import HistoryTable from '../components/HistoryTable';

function Dashboard() {
  const { data: latest, loading: loadingLatest, error: errorLatest } = usePolling(getLatestLectura, 10000);
  const { data: historial, loading: loadingHistorial, error: errorHistorial } = usePolling(getLecturas, 30000);

  if (loadingLatest && loadingHistorial) {
    return (
      <div className="loading-container">
        <div className="spinner"></div>
        <p>Cargando datos de sensores...</p>
      </div>
    );
  }

  return (
    <div className="dashboard">
      <header className="dashboard-header">
        <h1>Estación Meteorológica</h1>
        <p className="dashboard-subtitle">
          Monitoreo en tiempo real de sensores ambientales
          {latest && (
            <span className="update-time">
              {' '}· Última actualización: {new Date(latest.fecha_creacion).toLocaleTimeString('es-PE')}
            </span>
          )}
        </p>
      </header>

      {errorLatest && (
        <div className="error-banner">
          <span>⚠️</span> {errorLatest}
        </div>
      )}

      <section className="cards-grid">
        <SensorCard
          title="Temperatura Ambiente"
          value={latest?.temperatura_dht?.toFixed(1)}
          unit="°C"
          icon="🌡️"
          color="#ff6b6b"
        />
        <SensorCard
          title="Humedad Ambiente"
          value={latest?.humedad_ambiente?.toFixed(1)}
          unit="%"
          icon="💧"
          color="#4ecdc4"
        />
        <SensorCard
          title="Presión Atmosférica"
          value={latest?.presion ? (latest.presion / 100).toFixed(1) : null}
          unit="hPa"
          icon="🔵"
          color="#45b7d1"
        />
        <SensorCard
          title="Humedad del Suelo"
          value={latest?.humedad_suelo?.toFixed(1)}
          unit="%"
          icon="🌱"
          color="#96ceb4"
        />
        <SensorCard
          title="Estado del Suelo"
          value={latest?.estado_suelo}
          unit=""
          icon="🌿"
          color="#f1c40f"
        />
        <SensorCard
          title="Temp. BMP180"
          value={latest?.temperatura_bmp?.toFixed(1)}
          unit="°C"
          icon="🌤️"
          color="#e056fd"
        />
      </section>

      <section className="charts-grid">
        <SensorChart
          data={historial || []}
          dataKey="temperatura_dht"
          color="#ff6b6b"
          title="Temperatura Ambiente vs Tiempo"
          unit="°C"
        />
        <SensorChart
          data={historial || []}
          dataKey="humedad_ambiente"
          color="#4ecdc4"
          title="Humedad Ambiente vs Tiempo"
          unit="%"
        />
        <SensorChart
          data={historial || []}
          dataKey="presion"
          color="#45b7d1"
          title="Presión Atmosférica vs Tiempo"
          unit="hPa"
        />
        <SensorChart
          data={historial || []}
          dataKey="humedad_suelo"
          color="#96ceb4"
          title="Humedad del Suelo vs Tiempo"
          unit="%"
        />
      </section>

      <section className="history-section">
        <HistoryTable data={historial || []} />
      </section>
    </div>
  );
}

export default Dashboard;
