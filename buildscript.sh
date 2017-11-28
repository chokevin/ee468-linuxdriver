make all
sudo rmmod stackdriver.ko
sudo insmod stackdriver.ko
sudo chmod 666 /dev/stackdriver
./test
make clean

