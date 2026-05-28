from django.db import models


class LecturaSensor(models.Model):
    device_id = models.CharField(max_length=50)
    temperatura_dht = models.FloatField()
    humedad_ambiente = models.FloatField()
    temperatura_bmp = models.FloatField()
    presion = models.IntegerField()
    humedad_suelo = models.FloatField()
    estado_suelo = models.CharField(max_length=20)
    fecha_creacion = models.DateTimeField(auto_now_add=True)

    class Meta:
        ordering = ['-fecha_creacion']
        verbose_name = 'Lectura de Sensor'
        verbose_name_plural = 'Lecturas de Sensores'

    def __str__(self):
        return f'{self.device_id} - {self.fecha_creacion}'
