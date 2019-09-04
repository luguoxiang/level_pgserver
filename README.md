# level_pgserver
## Install
```
brew install flex bison (Mac)
sudo apt-get install flex bison (Linux)
git submodule update --init
cmake .
make
GLOG_logtostderr=1  bin/csv2pgserver 
```

## Download PostgreSQL client
Reference https://www.compose.com/articles/postgresql-tips-installing-the-postgresql-client/
```
psql -h localhost -p 5433
```
