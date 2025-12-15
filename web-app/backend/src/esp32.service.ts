import { Injectable, InternalServerErrorException } from '@nestjs/common';
import * as net from 'net';

@Injectable()
export class Esp32Service {
    private espIp = '192.168.0.29';
    private readonly espPort = 3333;

    setIp(ip: string) {
        this.espIp = ip
        return this.espIp
    }

    async sendCommand(payload: any): Promise<any> {
        return new Promise((resolve, reject) => {
            const socket = new net.Socket();

            socket.setTimeout(50000); 

            socket.connect(this.espPort, this.espIp, () => {
                console.log('Nest conectou ao ESP32 via TCP');
                
                const dataToSend = JSON.stringify(payload);
                socket.write(dataToSend);
            });

            socket.on('data', (data) => {
                const responseString = data.toString();
                console.log('Recebido do ESP:', responseString);
                
                try {
                    const jsonResponse = JSON.parse(responseString);
                    resolve(jsonResponse);
                } catch (e) {
                    resolve({ raw: responseString });
                }
                
                socket.destroy(); // Fecha a conexão (importante!)
            });

            socket.on('error', (err) => {
                console.error('Erro no Socket:', err.message);
                reject(new InternalServerErrorException('Falha ao conectar no ESP32'));
            });

            socket.on('timeout', () => {
                console.error('Timeout esperando o ESP32');
                socket.destroy();
                reject(new InternalServerErrorException('Timeout: Ninguém apertou o botão?'));
            });

        });
    }
}