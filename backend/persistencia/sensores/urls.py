from django.urls import path
from . import views

urlpatterns = [
    path('api/lecturas/', views.LecturaListCreateView.as_view(), name='lectura-list-create'),
    path('api/lecturas/latest/', views.LatestLecturaView.as_view(), name='lectura-latest'),
]
