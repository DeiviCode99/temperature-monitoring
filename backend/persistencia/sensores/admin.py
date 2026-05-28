from django.contrib import admin
from .models import LecturaSensor


@admin.register(LecturaSensor)
class LecturaSensorAdmin(admin.ModelAdmin):
    list_display = ('device_id', 'temperatura_dht', 'humedad_ambiente', 'presion', 'humedad_suelo', 'estado_suelo', 'fecha_creacion')
    list_filter = ('device_id', 'estado_suelo', 'fecha_creacion')
    search_fields = ('device_id',)
    readonly_fields = ('fecha_creacion',)
