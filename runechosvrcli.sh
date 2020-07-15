killall echosvr
killall echocli
g++ -o echosvr echosvr.cpp
g++ -o echocli echocli.cpp
./echosvr &
./echocli
