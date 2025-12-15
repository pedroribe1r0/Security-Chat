import { Body, Controller, Get, Post } from "@nestjs/common";
import { PoolMsgDTO, RegisterInDTO, SendMsgDTO, SetIpDTO } from "./dtos/register.dto";
import { Esp32Service } from "./esp32.service";
import { registerRes, sendMsgRes } from "./types";


@Controller('')
export class AppController {

    constructor(
        private readonly esp32Service: Esp32Service
    ){}

    @Post('ip')
    setIp(@Body() body: SetIpDTO) {
        return this.esp32Service.setIp(body.ip);
    }

    @Post('register')
    async register(@Body() body: RegisterInDTO): Promise<registerRes> {
        const payload = {
            header: "register",
            username: body.username
        };
        return await this.esp32Service.sendCommand(payload);
    }

    @Post('send')
    async sendMsg(@Body() body: SendMsgDTO): Promise<sendMsgRes>{
        //encrypt code
        const payload = {
            header: "send",
            username: body.username,
            hashCode: body.hashCode,
            msg: body.msg
        };

        return await this.esp32Service.sendCommand(payload);
    }

    @Post('pool')
    async pooling(@Body() body: PoolMsgDTO) {
        const payload = {
            header: "pool",
            username: body.username,
            hashCode: body.hashCode
        }

        return await this.esp32Service.sendCommand(payload);
    }
    
};