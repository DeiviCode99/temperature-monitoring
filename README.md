

# Weather Station - Estación Meteorológica IoT

Sistema completo de monitoreo ambiental en tiempo real basado en **ESP32**, ideal para agricultura de precisión, huertos urbanos, invernaderos y estaciones meteorológicas personales.

## Tabla de Contenidos

- [Arquitectura](#arquitectura)
- [Sensores](#sensores)
- [Requerimientos](#requerimientos)
- [Guía de Uso](#gu-a-de-uso)
  - [1. ESP32 (Firmware)](#1-esp32-firmware)
  - [2. Base de Datos (PostgreSQL)](#2-base-de-datos-postgresql)
  - [3. Backend (Django REST API)](#3-backend-django-rest-api)
  - [4. Serial Bridge (opcional)](#4-serial-bridge-opcional)
  - [5. Frontend (React + Vite)](#5-frontend-react--vite)
- [API Endpoints](#api-endpoints)
- [Despliegue](#despliegue)
- [Capturas](#capturas)
- [Contribuir](#contribuir)
- [Licencia](#licencia)

## Arquitectura

```
┌──────────────┐     WiFi/USB     ┌──────────────┐     HTTP      ┌──────────────┐
│   ESP32      │ ──────────────>  │  Django API  │ ────────────> │  PostgreSQL  │
│  (Sensores)  │                  │  (Backend)   │               │  (BD)        │
└──────────────┘                  └──────┬───────┘               └──────────────┘
                                         │
                                         │ REST
                                         ▼
                                  ┌──────────────┐
                                  │   React App  │
                                  │  (Frontend)  │
                                  └──────────────┘
```

El flujo de datos funciona de la siguiente manera:

1. **ESP32** lee los sensores cada 3 segundos y envía las lecturas vía HTTP (WiFi) al backend Django.
2. **Serial Bridge** (alternativa por USB) captura el JSON que el ESP32 imprime por el puerto serie y lo reenvía al mismo backend.
3. **Backend Django** recibe, autentica y almacena cada lectura en PostgreSQL.
4. **Frontend React** consulta la API cada 10 segundos y muestra los datos en un dashboard con tarjetas, gráficos y tabla histórica.

## Sensores

| Sensor      | Magnitud                     | Rango típico       |
|-------------|------------------------------|--------------------|
| DHT11       | Temperatura ambiente         | 0-50 °C            |
| DHT11       | Humedad ambiente             | 20-90 %            |
| BMP180      | Temperatura barométrica      | -40-85 °C          |
| BMP180      | Presión atmosférica          | 300-1100 hPa       |
| FC-28       | Humedad del suelo            | 0-100 %            |

## Requerimientos

- **ESP32** con los sensores DHT11, BMP180 y FC-28
- Pantalla LCD 16x2 con interfaz I2C
- **Python 3.10+** y **Node.js 18+**
- **PostgreSQL 14+**
- Conexión WiFi para el ESP32 (o cable USB para el Serial Bridge)

## Guía de Uso

### 1. ESP32 (Firmware)

1. Abre `esp32/esp32_weather_station.ino` en Arduino IDE.
2. Instala las librerías necesarias desde el Gestor de Librerías:
   - `DHT sensor library` de Adafruit
   - `Adafruit BMP085 Library`
   - `hd44780` de Bill Perry
   - `ArduinoJson` de Benoit Blanchon
3. Configura tu red WiFi y la URL del servidor en el código:
   ```cpp
   const char* ssid = "TU_RED_WIFI";
   const char* password = "TU_CONTRASEÑA";
   const char* serverUrl = "http://IP_DEL_SERVIDOR:8000/api/lecturas/";
   ```
4. Conecta los componentes según el siguiente cableado:

   | Componente | Pin ESP32 |
   |------------|-----------|
   | DHT11      | GPIO 4    |
   | FC-28      | GPIO 34   |
   | LCD I2C    | SDA=18, SCL=19 |
   | BMP180     | SDA=21, SCL=22 |

5. Carga el firmware al ESP32.

### 2. Base de Datos (PostgreSQL)

```bash
psql -U postgres
CREATE DATABASE temperatura_db;
\q
```

### 3. Backend (Django REST API)

```bash
cd backend

# Crear y activar entorno virtual
python -m venv env
.\env\Scripts\activate   # Windows
source env/bin/activate  # Linux/Mac

# Instalar dependencias
pip install -r requirements.txt

# Configurar variables de entorno
cp .env.example .env
# Edita .env con tus datos de base de datos

# Migrar y ejecutar
python persistencia/manage.py migrate
python persistencia/manage.py runserver 0.0.0.0:8000
```

### 4. Serial Bridge (opcional)

Si tu ESP32 está conectado por USB en lugar de WiFi:

```bash
cd backend
python serial_bridge.py
```

El script detecta automáticamente el puerto del ESP32 y reenvía las lecturas al backend.

### 5. Frontend (React + Vite)

```bash
cd frontend/clime-monitor

npm install
npm run dev
```

El frontend estará disponible en `http://localhost:5173`. Asegúrate de configurar `VITE_API_URL` en un archivo `.env` si el backend corre en otra dirección.

## API Endpoints

| Método | Endpoint                  | Descripción                     |
|--------|---------------------------|---------------------------------|
| GET    | `/api/lecturas/`          | Lista todas las lecturas        |
| POST   | `/api/lecturas/`          | Crea una nueva lectura          |
| GET    | `/api/lecturas/latest/`   | Obtiene la última lectura       |

Todas las peticiones deben incluir el header:

```
X-API-Key: TU_API_KEY
```

## Despliegue en Producción

### Backend

```bash
cd backend
gunicorn persistencia.wsgi:application --bind 0.0.0.0:8000 --workers 4
```

### Frontend

```bash
cd frontend/clime-monitor
npm run build
```

Sirve la carpeta `dist/` con Nginx, Apache o súbela a Vercel/Netlify.

### ESP32 en campo

Asegúrate de que el ESP32 tenga una fuente de alimentación estable (5V, 2A recomendado) y que esté al alcance de la red WiFi. Para exteriores, considera una caja estanca.

## Contribuir

Las contribuciones son bienvenidas. Por favor, abre un issue o un pull request para sugerir cambios.

## Licencia

MIT
