function SensorCard({ title, value, unit, icon, color }) {
  return (
    <div className="sensor-card" style={{ borderTop: `4px solid ${color}` }}>
      <div className="sensor-card-header">
        <span className="sensor-icon">{icon}</span>
        <span className="sensor-title">{title}</span>
      </div>
      <div className="sensor-value">
        {value !== null && value !== undefined ? value : '---'}
        <span className="sensor-unit"> {unit}</span>
      </div>
    </div>
  );
}

export default SensorCard;
