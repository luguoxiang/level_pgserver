# cvs2pgserver
brew install flex bison (Mac)
sudo apt-get install flex bison (Linux)
git submodule update --init
cmake .
make
GLOG_logtostderr=1  bin/csv2pgserver 
