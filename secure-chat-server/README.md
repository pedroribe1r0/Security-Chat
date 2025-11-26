## Comandos ##

--Habilitar o esp-idf
cd ~/esp/esp-idf
. export.sh


--criar projeto
idf.py create-project <nome>

--resolver problemas de import
ESP-IDF:add VS Code Configuration Folder

--compilar
idf.py build

--passar o codigo para o esp32
idf.py -p /dev/ttyUSB0 flash

--abrir o monitor serial 
idf.py -p /dev/ttyUSB0 monitor
