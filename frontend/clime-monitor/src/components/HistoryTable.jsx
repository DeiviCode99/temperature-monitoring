function HistoryTable({ data }) {
  if (!data || data.length === 0) {
    return (
      <div className="history-empty">
        <p>No hay datos históricos disponibles</p>
      </div>
    );
  }

  return (
    <div className="history-table-container">
      <h3 className="history-title">Historial de Lecturas</h3>
      <div className="table-wrapper">
        <table className="history-table">
          <thead>
            <tr>
              <th>Fecha</th>
              <th>Temp. DHT (°C)</th>
              <th>Humedad (%)</th>
              <th>Presión (hPa)</th>
              <th>Humedad Suelo (%)</th>
              <th>Estado</th>
            </tr>
          </thead>
          <tbody>
            {data.slice(0, 50).map((lectura) => (
              <tr key={lectura.id}>
                <td>
                  {new Date(lectura.fecha_creacion).toLocaleString('es-PE', {
                    day: '2-digit',
                    month: '2-digit',
                    hour: '2-digit',
                    minute: '2-digit',
                  })}
                </td>
                <td>{lectura.temperatura_dht?.toFixed(1)}</td>
                <td>{lectura.humedad_ambiente?.toFixed(1)}</td>
                <td>{(lectura.presion / 100)?.toFixed(1)}</td>
                <td>{lectura.humedad_suelo?.toFixed(1)}</td>
                <td>
                  <span className={`status-badge status-${lectura.estado_suelo?.toLowerCase() || 'unknown'}`}>
                    {lectura.estado_suelo || '---'}
                  </span>
                </td>
              </tr>
            ))}
          </tbody>
        </table>
      </div>
    </div>
  );
}

export default HistoryTable;
