type registerRes = {
    hashCode?: string,

    encryptHashCode?: string,

    error?: string
}

type sendMsgRes = {
    status?: string,

    msg?: string,

    error?: string
}

export type { registerRes, sendMsgRes }