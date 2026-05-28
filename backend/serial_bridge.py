import sys
import json
import time
import requests
import serial
import serial.tools.list_ports

API_URL = "http://127.0.0.1:8000/api/lecturas/"
API_KEY = "ESP32_API_KEY_2024"
BAUD = 115200


def encontrar_esp32():
    puertos = list(serial.tools.list_ports.comports())
    for p in puertos:
        if "USB" in p.description or "CH340" in p.description or "CP210" in p.description or "Arduino" in p.description:
            print(f"ESP32 encontrado en: {p.device} ({p.description})")
            return p.device
    print("No se encontró un ESP32 automáticamente.")
    print("Puertos disponibles:")
    for p in puertos:
        print(f"  {p.device} - {p.description}")
    if puertos:
        return puertos[0].device
    return None


def enviar_a_django(datos):
    try:
        resp = requests.post(
            API_URL,
            json=datos,
            headers={"X-API-Key": API_KEY},
            timeout=5
        )
        print(f"[{time.strftime('%H:%M:%S')}] HTTP {resp.status_code} - {datos.get('temperatura_dht', '?')}C")
        return True
    except requests.exceptions.ConnectionError:
        print(f"[{time.strftime('%H:%M:%S')}] ERROR: Django no disponible en {API_URL}")
        return False
    except Exception as e:
        print(f"[{time.strftime('%H:%M:%S')}] ERROR: {e}")
        return False


def main():
    puerto = encontrar_esp32()
    if not puerto:
        print("No se pudo determinar el puerto. Especifica manualmente:")
        print("  python serial_bridge.py COM3")
        if len(sys.argv) > 1:
            puerto = sys.argv[1]
        else:
            sys.exit(1)

    print(f"Conectando a {puerto}...")
    try:
        ser = serial.Serial(puerto, BAUD, timeout=2)
    except serial.SerialException as e:
        print(f"Error al abrir {puerto}: {e}")
        sys.exit(1)

    print("Leyendo datos del ESP32...")
    buffer = ""
    while True:
        try:
            dato = ser.read().decode("utf-8", errors="ignore")
            if dato == "":
                continue
            buffer += dato
            if dato == "\n":
                linea = buffer.strip()
                buffer = ""
                if linea.startswith("{"):
                    try:
                        datos = json.loads(linea)
                        enviar_a_django(datos)
                    except json.JSONDecodeError:
                        pass
        except KeyboardInterrupt:
            print("\nDeteniendo...")
            break
        except serial.SerialException:
            print("Conexión perdida. Reconectando...")
            time.sleep(2)
            try:
                ser = serial.Serial(puerto, BAUD, timeout=2)
            except serial.SerialException:
                break

    ser.close()


if __name__ == "__main__":
    main()
