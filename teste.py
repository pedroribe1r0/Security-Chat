import socket
import json
import sys
import time
import threading

# --- CONFIGURAÇÕES ---
SERVER_IP = '192.168.0.29'  # <--- COLOQUE SEU IP AQUI
SERVER_PORT = 3333
POOL_INTERVAL = 2           # Tempo em segundos entre atualizações (Pooling)

# Cores para o terminal ficarem bonitos
class Colors:
    HEADER = '\033[95m'
    BLUE = '\033[94m'
    GREEN = '\033[92m'
    YELLOW = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'

# Estado Global
my_username = ""
my_hash = ""
seen_msg_ids = set() # Para não repetir mensagens que já mostramos
running = True

def send_request(payload, timeout=5):
    """Abre socket, manda JSON, recebe resposta e fecha socket."""
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(timeout)
        s.connect((SERVER_IP, SERVER_PORT))
        
        s.sendall(json.dumps(payload).encode('utf-8'))
        
        # Buffer grande para histórico
        data = s.recv(4096) 
        s.close()
        
        if not data: return None
        return json.loads(data.decode('utf-8'))
        
    except Exception as e:
        # Silencia erros de timeout no pooling para não sujar a tela
        if "timed out" not in str(e):
            # print(f"{Colors.FAIL}Erro de conexão: {e}{Colors.ENDC}")
            pass
        return None

def pooling_thread():
    """Fica rodando em background pedindo novas mensagens"""
    global seen_msg_ids
    
    while running:
        if not my_hash: 
            time.sleep(1)
            continue

        payload = {
            "header": "pool",
            "username": my_username,
            "hashCode": my_hash
        }
        
        resp = send_request(payload, timeout=3)
        
        if resp and isinstance(resp, list):
            # Ordena por ID para mostrar na ordem certa
            # O ID vem do C como número
            try:
                msgs = sorted(resp, key=lambda x: x['id'])
                
                for m in msgs:
                    msg_id = m.get('id')
                    sender = m.get('user')
                    content = m.get('msg')
                    
                    if msg_id not in seen_msg_ids:
                        seen_msg_ids.add(msg_id)
                        
                        # Formatação bonita
                        if sender == my_username:
                            # Minhas mensagens (ignora ou mostra diferente)
                            pass 
                        else:
                            # Mensagem dos outros
                            print(f"\r{Colors.YELLOW}[{sender}]: {content}{Colors.ENDC}")
                            # Restaura o prompt de input visualmente
                            print(f"{Colors.BLUE}[Você]: {Colors.ENDC}", end="", flush=True)
            except:
                pass

        time.sleep(POOL_INTERVAL)

def main():
    global my_username, my_hash, running

    print(f"{Colors.HEADER}=== ESP32 SECURE CHAT ==={Colors.ENDC}")
    print(f"Conectando em {SERVER_IP}:{SERVER_PORT}")

    # --- 1. REGISTRO ---
    while True:
        my_username = input("Escolha seu Username: ").strip()
        if not my_username: continue
        
        print(f"{Colors.BOLD}Solicitando registro... OLHE PARA O LCD DO ESP32!{Colors.ENDC}")
        print("Aguardando botão (20s timeout)...")
        
        payload = {"header": "register", "username": my_username}
        resp = send_request(payload, timeout=25) # Tempo longo para o humano
        
        if resp and "hashPass" in resp:
            my_hash = resp['hashPass']
            print(f"{Colors.GREEN}✅ Acesso Permitido! Token: {my_hash}{Colors.ENDC}")
            break
        elif resp and "error" in resp:
            print(f"{Colors.FAIL}❌ Erro: {resp['error']}{Colors.ENDC}")
            if "registered" in resp['error']:
                print("Tente outro nome.")
        else:
            print(f"{Colors.FAIL}❌ Sem resposta ou Recusado pelo Admin.{Colors.ENDC}")
            return

    # --- 2. INICIA POOLING (RECEBIMENTO) ---
    t = threading.Thread(target=pooling_thread)
    t.daemon = True # Mata a thread se o programa fechar
    t.start()
    
    print("-" * 40)
    print("Chat iniciado! Digite e dê Enter para enviar.")
    print("-" * 40)

    # --- 3. LOOP DE ENVIO ---
    try:
        while True:
            msg = input(f"{Colors.BLUE}[Você]: {Colors.ENDC}")
            if not msg: continue
            
            # Limpa a linha anterior para não ficar duplicado (estético)
            sys.stdout.write("\033[F") # Cursor up one line
            sys.stdout.write("\033[K") # Clear line
            print(f"{Colors.GREEN}[Você]: {msg}{Colors.ENDC}")

            payload = {
                "header": "send", # Verifique se no C está "send" ou "send-msg"
                "username": my_username,
                "hashCode": my_hash,
                "msg": msg
            }
            
            send_request(payload)
            
    except KeyboardInterrupt:
        print("\nSaindo...")
        running = False

if __name__ == "__main__":
    main()