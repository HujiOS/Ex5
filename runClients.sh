make clean
make
xterm -e ./WhatsappClient a 127.0.0.1 8888 &
xterm -e ./WhatsappClient b 127.0.0.1 8888 &
xterm -e ./WhatsappClient c 127.0.0.1 8888 
