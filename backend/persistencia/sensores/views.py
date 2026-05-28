from rest_framework import generics, status
from rest_framework.response import Response
from rest_framework.views import APIView
from .models import LecturaSensor
from .serializers import LecturaSensorSerializer
from .authentication import APIKeyAuthentication


class LecturaListCreateView(generics.ListCreateAPIView):
    queryset = LecturaSensor.objects.all()
    serializer_class = LecturaSensorSerializer
    authentication_classes = [APIKeyAuthentication]

    def perform_create(self, serializer):
        serializer.save()


class LatestLecturaView(APIView):
    authentication_classes = [APIKeyAuthentication]

    def get(self, request):
        lectura = LecturaSensor.objects.order_by('-fecha_creacion').first()
        if lectura:
            serializer = LecturaSensorSerializer(lectura)
            return Response(serializer.data)
        return Response({'detail': 'No hay lecturas disponibles'}, status=status.HTTP_404_NOT_FOUND)
