# level_pgserver
## Install
```
brew install flex bison (Mac)
sudo apt-get install flex bison (Linux)
git submodule update --init
cmake --build . --target level_pgserver 
GLOG_logtostderr=1  bin/level_pgserver 
```

## Download PostgreSQL client
Reference https://www.compose.com/articles/postgresql-tips-installing-the-postgresql-client/
```
psql -h localhost -p 5433
```
