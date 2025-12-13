import socket
import json
import sys
import time

# --- CONFIGURAÇÕES ---
SERVER_IP = '192.168.0.29'  # Seu IP
SERVER_PORT = 3333          
BUFFER_SIZE = 1024          

# Nome do usuário de teste
TEST_USER = "tester_01"

def send_request(payload, description, timeout=5):
    """Função genérica para abrir socket, enviar JSON e receber resposta."""
    try:
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        #client_socket.settimeout(timeout) # Timeout importante!
        
        print(f"\n[{description}] Conectando a {SERVER_IP}:{SERVER_PORT}...")
        client_socket.connect((SERVER_IP, SERVER_PORT))
        
        json_payload = json.dumps(payload)
        print(f"[{description}] Enviando: {json_payload}")
        client_socket.sendall(json_payload.encode('utf-8'))
        
        # Espera resposta
        print(f"[{description}] Aguardando resposta...")
        data = client_socket.recv(BUFFER_SIZE)
        
        client_socket.close()
        
        if data:
            response_str = data.decode('utf-8')
            print(f"[{description}] Resposta Raw: {response_str}")
            return json.loads(response_str)
        else:
            print(f"[{description}] Sem resposta (Conexão fechada pelo servidor).")
            return None

    except socket.timeout:
        print(f"❌ [{description}] Timeout! (Você demorou para apertar o botão?)")
        return None
    except Exception as e:
        print(f"❌ [{description}] Erro: {e}")
        return None

def run_test():
    # --- PASSO 1: REGISTRAR (Precisa apertar o botão no ESP32!) ---
    print("="*40)
    print("ETAPA 1: REGISTRO (Prepare-se para apertar o botão!)")
    print("="*40)
    
    reg_payload = {
        "header": "register",
        "username": TEST_USER
    }
    
    # Timeout de 20 segundos para dar tempo de você ler o LCD e apertar o botão
    reg_response = send_request(reg_payload, "REGISTER", timeout=20)
    
    if not reg_response:
        return # Falha fatal

    if "error" in reg_response:
        print(f"❌ Erro no registro: {reg_response['error']}")
        # Se o erro for "User already registered", podemos tentar prosseguir se soubermos o hash,
        # mas para teste é melhor reiniciar o ESP32 ou mudar o nome do usuário.
        return

    # Pega o hash retornado (pode ser hashPass ou encryptHashCode dependendo do seu repo)
    # Baseado no seu código anterior: cJSON_AddStringToObject(res, "hashPass", hash);
    user_hash = reg_response.get("hashPass")
    
    if not user_hash:
        print("❌ Erro: Servidor não retornou 'hashPass'.")
        return

    print(f"✅ Registro Sucesso! Hash recebido: {user_hash}")
    print("Esperando 2 segundos antes de mandar mensagem...")
    time.sleep(2)

    # --- PASSO 2: ENVIAR MENSAGEM ---
    print("\n" + "="*40)
    print("ETAPA 2: ENVIAR MENSAGEM")
    print("="*40)

    msg_payload = {
        "header": "send",     # Verifique se no socket-server.c está "send-msg" ou "chat"
        "username": TEST_USER,
        "hashCode": "user_hash",    # A senha que ganhamos no passo 1
        "msg": "Ola ESP32, funcionando!"
    }

    msg_response = send_request(msg_payload, "SEND_MSG", timeout=5)

    if msg_response:
        if "error" in msg_response:
            print(f"❌ Falha ao enviar msg: {msg_response['error']}")
        else:
            print("✅ SUCESSO! Mensagem enviada e salva.")
            print(f"Status: {msg_response.get('status')}")
            print(f"Msg ecoada: {msg_response.get('msg')}")

if __name__ == '__main__':
    run_test()