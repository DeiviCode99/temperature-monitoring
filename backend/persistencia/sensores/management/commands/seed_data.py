import random
from datetime import timedelta
from django.core.management.base import BaseCommand
from django.utils import timezone
from sensores.models import LecturaSensor


class Command(BaseCommand):
    help = 'Genera datos de prueba para las lecturas de sensores'

    def add_arguments(self, parser):
        parser.add_argument('--lecturas', type=int, default=100, help='Numero de lecturas a generar')

    def handle(self, *args, **options):
        count = options['lecturas']
        estados = ['Seco', 'Humedo', 'Mojado']
        dispositivos = ['ESP32_01', 'ESP32_02']

        LecturaSensor.objects.all().delete()
        self.stdout.write(f'Generando {count} lecturas de prueba...')

        now = timezone.now()
        lecturas = []

        for i in range(count):
            timestamp = now - timedelta(minutes=i * 5, seconds=random.randint(0, 300))
            temp_dht = round(random.uniform(18.0, 35.0), 1)
            humedad = round(random.uniform(40.0, 95.0), 1)
            temp_bmp = round(temp_dht + random.uniform(-1, 1), 1)
            presion = random.randint(100500, 102500)
            humedad_suelo = round(random.uniform(10.0, 90.0), 1)

            if humedad_suelo > 70:
                estado = 'Mojado'
            elif humedad_suelo > 30:
                estado = 'Humedo'
            else:
                estado = 'Seco'

            lecturas.append(LecturaSensor(
                device_id=random.choice(dispositivos),
                temperatura_dht=temp_dht,
                humedad_ambiente=humedad,
                temperatura_bmp=temp_bmp,
                presion=presion,
                humedad_suelo=humedad_suelo,
                estado_suelo=estado,
                fecha_creacion=timestamp,
            ))

        LecturaSensor.objects.bulk_create(lecturas)
        self.stdout.write(self.style.SUCCESS(f'[OK] {count} lecturas de prueba creadas exitosamente'))
