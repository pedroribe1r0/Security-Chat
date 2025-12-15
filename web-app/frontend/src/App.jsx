import { useEffect, useRef, useState } from "react"
import axios from "axios"
import "./App.css"

export default function App() {
    const [view, setView] = useState('login') 
    const [errorMessage, setErrorMessage] = useState("")
    
    const [sendMessage, setSendMessage] = useState("")
    const [loginInfo, setLoginInfo] = useState({
        ip: "",
        username: ""
    })
    const [messages, setMessages] = useState([])
    
    const codes = useRef({
        hashCode: "",
        encryptHashCode: ""
    })

    const messagesEndRef = useRef(null)
    const baseUrl = "http://localhost:3000/api/"

    const scrollToBottom = () => {
        messagesEndRef.current?.scrollIntoView({ behavior: "smooth" })
    }

    useEffect(() => {
        scrollToBottom()
    }, [messages])

    const handleBack = () => {
        setView('login');
        setMessages([]);
        setErrorMessage("");
        codes.current = { hashCode: "", encryptHashCode: "" };
    }

    const handleLogin = async (e) => {
        if(e) e.preventDefault(); 
        
        setView('waiting');
        setErrorMessage("");

        try {
            await axios.post(
                `${baseUrl}ip`,
                { ip: loginInfo.ip }
            )

            const loginRes = await axios.post(
                `${baseUrl}register`,
                { username: loginInfo.username }
            )
            
            if(loginRes.data && loginRes.data.error){
                setErrorMessage(loginRes.data.error);
                setView('refused');
                return
            }
            
            codes.current = {
                hashCode: loginRes.data?.hashPass, 
                encryptHashCode: loginRes.data?.encryptHashCode
            }
            setView('chat');
        }
        catch(e) {
            setErrorMessage("Server Unreachable or Timeout");
            setView('refused');
        }
    }

    useEffect(() => {
        if (view !== 'chat') return; 
        if (!codes.current.hashCode) return;

        const interval = setInterval(async () => {
            try {
                const res = await axios.post(
                    `${baseUrl}pool`,
                    { 
                        username: loginInfo.username, 
                        hashCode: codes.current.hashCode
                    }
                )

                if (res.data.error) return;

                setMessages(res.data)
            } catch (e) {
                console.error(e)
            }
        }, 5000)

        return () => clearInterval(interval)
    }, [view, loginInfo.username])

    const handleSendMsg = async (e) => {
        e.preventDefault()
        if(!sendMessage.trim()) return

        try {   
            const res = await axios.post(
                `${baseUrl}send`,
                { 
                    username: loginInfo.username, 
                    hashCode: codes.current.hashCode, 
                    msg: sendMessage, 
                    encryptHashCode: codes.current.encryptHashCode 
                }
            )

            if(res.data.error){
                console.log(res.data.error)
                return
            }

            setMessages(prev => ([
                ...prev,
                {
                    user: loginInfo.username,
                    msg: res.data.msg
                }
            ]))

            setSendMessage("")
        }
        catch(e) {
            console.error(e)
        }
    }

    return(
        <>  
            {view === 'login' && (
                <div className="full-screen-center">
                    <form className="login-card" onSubmit={ handleLogin }>
                        <h1 className="login-title"><span></span>Secure Chat</h1>

                        <div className="input-group">
                            <label htmlFor="ip">Target Server</label>
                            <input 
                                type="text" 
                                id="ip" 
                                placeholder="192.168.0.XX" 
                                value={loginInfo.ip} 
                                onChange={(e) => setLoginInfo(prev => ({ ...prev, ip: e.target.value}))}
                                autoComplete="off"
                            />
                        </div>

                        <div className="input-group">
                            <label htmlFor="username">Operator ID</label>
                            <input 
                                type="text" 
                                id="username" 
                                placeholder="Username" 
                                value={loginInfo.username} 
                                onChange={(e) => setLoginInfo(prev => ({ ...prev, username: e.target.value}))}
                                autoComplete="off"
                            />
                        </div>

                        <button type="submit" className="btn-login">Initialize</button>
                    </form>
                </div>
            )}

            {view === 'waiting' && (
                <div className="full-screen-center">
                    <button className="btn-back" onClick={handleBack} title="Cancel">
                        <svg viewBox="0 0 24 24"><path d="M20 11H7.83l5.59-5.59L12 4l-8 8 8 8 1.41-1.41L7.83 13H20v-2z"/></svg>
                    </button>

                    <div className="waiting-container">
                        <div className="spinner"></div>
                        <p className="waiting-text">Awaiting Authorization...</p>
                    </div>
                </div>
            )}

            {view === 'refused' && (
                <div className="full-screen-center">
                    <div className="refused-container">
                        <svg className="refused-icon" viewBox="0 0 24 24" fill="currentColor">
                            <path d="M12 2C6.48 2 2 6.48 2 12s4.48 10 10 10 10-4.48 10-10S17.52 2 12 2zm1 15h-2v-2h2v2zm0-4h-2V7h2v6z"/>
                        </svg>
                        <h2 className="refused-title">Access Denied</h2>
                        <p className="refused-msg">
                            {errorMessage || "Connection rejected by terminal administrator."}
                        </p>
                        <button className="btn-retry" onClick={handleBack}>
                            Return to Login
                        </button>
                    </div>
                </div>
            )}

            {view === 'chat' && (
                <div className="full-screen-center">
                    <button className="btn-back" onClick={handleBack} title="Disconnect">
                        <svg viewBox="0 0 24 24"><path d="M20 11H7.83l5.59-5.59L12 4l-8 8 8 8 1.41-1.41L7.83 13H20v-2z"/></svg>
                    </button>

                    <div className="chat-window">
                        <div className="chat-header">
                            <h1>Encrypted Feed // <span>{loginInfo.username}</span></h1>
                            <div className="status-indicator">SECURE</div>
                        </div>

                        <div className="chat-history">
                            {messages.map((msg, index) => {
                                const isMe = msg.user === loginInfo.username
                                return (
                                    <div key={index} className={`msg-row ${isMe ? 'mine' : 'other'}`}>
                                        <span className="msg-user">{isMe ? 'ME' : msg.user}</span>
                                        <div className="msg-content">{msg.msg}</div>
                                    </div>
                                )
                            })}
                            <div ref={messagesEndRef} />
                        </div>

                        <form className="chat-input-form" onSubmit={ handleSendMsg }>
                            <div className="input-wrapper">
                                <input 
                                    type="text" 
                                    placeholder="Enter command..." 
                                    value={sendMessage} 
                                    onChange={ (e) => setSendMessage(e.target.value)} 
                                    autoFocus
                                />
                                <button className="btn-send-icon" type="submit">
                                    <svg viewBox="0 0 24 24" width="24" height="24" fill="currentColor"><path d="M2.01 21L23 12 2.01 3 2 10l15 2-15 2z" /></svg>
                                </button>
                            </div>
                        </form>
                    </div>
                </div>
            )}
        </>
    )
}