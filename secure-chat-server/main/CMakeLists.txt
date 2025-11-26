idf_component_register(SRCS 
                    "secure-chat-server.c"
                    "./src/socket-server.c"
                    "./src/wifi-connect.c"
                    "./src/register-service.c"
                    "./src/user-repository.c"

                    INCLUDE_DIRS 
                    "." 
                    "include")
