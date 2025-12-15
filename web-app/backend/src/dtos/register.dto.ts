import { IsString } from "class-validator";

export class RegisterInDTO {
    @IsString()
    username: string;
};

export class SendMsgDTO {
    @IsString()
    username: string;

    @IsString()
    msg: string;

    @IsString()
    hashCode: string

    @IsString()
    encryptHashCode: string
}

export class PoolMsgDTO {
    @IsString()
    username: string;

    @IsString()
    hashCode: string;
}

export class SetIpDTO {
    @IsString()
    ip: string
}

// char header[16];
    // char username[32];
    // char hashCode[64];
    // char msg[256];
    // int client_sock;