make clean
make
xterm -e ./whatsappClient a 127.0.0.1 8888 &
xterm -e ./whatsappClient b 127.0.0.1 8888 &
xterm -e ./whatsappClient d 127.0.0.1 8888 &
xterm -e ./whatsappClient e 127.0.0.1 8888 &
xterm -e ./whatsappClient f 127.0.0.1 8888 &
xterm -e ./whatsappClient g 127.0.0.1 8888 &
xterm -e ./whatsappClient c 127.0.0.1 8888
