import socket
import json
import sys

# --- CONFIGURAÇÕES DO SERVIDOR ESP32 ---
SERVER_IP = '192.168.0.2'  # O IP estático que você configurou no ESP32
SERVER_PORT = 3333           # A porta que seu servidor ESP32 está escutando
BUFFER_SIZE = 1024           # Tamanho do buffer para a resposta

def test_server_connection():
    """Tenta conectar ao servidor, enviar JSON e receber uma resposta."""
    
    # Cria uma mensagem JSON de teste (simulando um login)
    test_message = {
        "header": "register",
        "username": "tester_client",
    }
    
    # Serializa o objeto Python (dict) em uma string JSON
    json_payload = json.dumps(test_message)
    
    # Cria o socket TCP (AF_INET para IPv4, SOCK_STREAM para TCP)
    try:
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    except socket.error as e:
        print(f"Erro ao criar o socket: {e}")
        sys.exit(1)

    print(f"Tentando conectar a {SERVER_IP}:{SERVER_PORT}...")

    # Conecta ao servidor
    try:
        # Define um timeout de 5 segundos para a conexão e operações
        client_socket.settimeout(5)
        client_socket.connect((SERVER_IP, SERVER_PORT))
        print("✅ Conexão estabelecida com sucesso!")
        
        # Envia a string JSON (codificada em bytes)
        print(f"Enviando dados: {json_payload}")
        client_socket.sendall(json_payload.encode('utf-8'))
        
        # Recebe a resposta do servidor
        data = client_socket.recv(BUFFER_SIZE)
        
        if data:
            response = data.decode('utf-8')
            print("-" * 30)
            print("Resposta do Servidor (ESP32):")
            print(response)
            
            # Tenta analisar a resposta como JSON (se o servidor responder em JSON)
            try:
                json_response = json.loads(response.strip())
                print("\nResposta JSON Analisada:")
                print(json_response)
            except json.JSONDecodeError:
                print("\nResposta não é um JSON válido.")
        else:
            print("Servidor fechou a conexão antes de enviar uma resposta.")

    except socket.timeout:
        print(f"❌ Erro: Timeout durante a conexão ou operação. Verifique a rede.")
    except ConnectionRefusedError:
        print(f"❌ Erro: Conexão Recusada. O servidor {SERVER_IP}:{SERVER_PORT} não está ativo ou escutando.")
    except socket.error as e:
        print(f"❌ Ocorreu um erro no socket: {e}")
    finally:
        # Fecha a conexão
        client_socket.close()
        print("\nConexão fechada.")


if __name__ == '__main__':
    test_server_connection()