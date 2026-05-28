from rest_framework.authentication import BaseAuthentication
from rest_framework.exceptions import AuthenticationFailed
from decouple import config


class APIKeyAuthentication(BaseAuthentication):
    def authenticate(self, request):
        api_key = request.headers.get('X-API-Key')
        expected_key = config('API_KEY', default='ESP32_API_KEY_2024')

        if request.method != 'GET':
            if not api_key:
                raise AuthenticationFailed('API Key requerida')
            if api_key != expected_key:
                raise AuthenticationFailed('API Key inválida')

        return None
