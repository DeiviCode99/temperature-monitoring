import {
  LineChart,
  Line,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  ResponsiveContainer,
  Legend,
} from 'recharts';

function SensorChart({ data, dataKey, color, title, unit }) {
  if (!data || data.length === 0) {
    return (
      <div className="chart-container">
        <h3 className="chart-title">{title}</h3>
        <div className="chart-empty">
          <p>Esperando datos...</p>
        </div>
      </div>
    );
  }

  const formattedData = data
    .slice()
    .reverse()
    .slice(-20)
    .map((item) => ({
      ...item,
      fecha: item.fecha_creacion
        ? new Date(item.fecha_creacion).toLocaleTimeString('es-PE', {
            hour: '2-digit',
            minute: '2-digit',
          })
        : '',
    }));

  return (
    <div className="chart-container">
      <h3 className="chart-title">{title}</h3>
      <ResponsiveContainer width="100%" height={250}>
        <LineChart data={formattedData}>
          <CartesianGrid strokeDasharray="3 3" stroke="#2a2a5a" />
          <XAxis dataKey="fecha" fontSize={12} stroke="#8888aa" />
          <YAxis fontSize={12} stroke="#8888aa" unit={unit ? ` ${unit}` : ''} />
          <Tooltip
            contentStyle={{
              background: '#1a1a3e',
              border: '1px solid #2a2a5a',
              borderRadius: '8px',
              color: '#e0e0e0',
            }}
          />
          <Legend />
          <Line
            type="monotone"
            dataKey={dataKey}
            stroke={color}
            strokeWidth={2}
            dot={false}
            activeDot={{ r: 5 }}
            name={title}
          />
        </LineChart>
      </ResponsiveContainer>
    </div>
  );
}

export default SensorChart;
