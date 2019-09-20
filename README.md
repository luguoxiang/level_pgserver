# Introduction

LevelPgServer is simple database that support limited SQL. 

* Use Postgress communication protocol
* Using leveldb as storage layer, also support read only table backed by CSV file.
* Simple query optimization with explain
* Asynchronized IO with Boost asio
* Build by modern C++ 17

# Quick start
## Install
```
brew install flex bison (Mac)
sudo apt-get install flex bison (Linux)
git submodule update --init
cmake -DCMAKE_BUILD_TYPE=Release .
cmake --build . --target level_pgserver 
GLOG_logtostderr=1  bin/level_pgserver 
```

## Define meta.conf file
```
[table]
name=test
path=testdb #directory holding leveldb data files, can also be a csv file path
column=a:int32
column=b:varchar
column=c:double
column=d:datetime
key=a,b #only valid for non-csv table
```

## Download PostgreSQL client
Reference https://www.compose.com/articles/postgresql-tips-installing-the-postgresql-client/
```
psql -h localhost -p 5433
psql (11.4, server 9.0.4)
Type "help" for help.

user=> show tables;
 TableName | Info
-----------+------
 alltypes  |
 querytest |
 test      |
(3 rows)

user=> desc test;
 Field |   Type   | KeyPosition | Precision
-------+----------+-------------+-----------
 a     | int32    |           0 |        -1
 b     | varchar  |           1 |        -1
 c     | double   |          -1 |        -1
 d     | datetime |          -1 |        -1
(4 rows)

user=> select * from test;
  a  |    b     |      c      |          d
-----+----------+-------------+---------------------
   0 | test-0   |    0.000000 | 2019-01-20 00:00:00
   1 | test-1   |    1.500000 | 2019-02-20 00:00:00
   2 | test-2   |    3.000000 | 2019-03-20 00:00:00
...

user=> explain select * from test where a >= 1;
                             explain
------------------------------------------------------------------
 Projection a, b, c, d,
         leveldb:scan test, cost:260522
                 range [int:1|string:, int:2147483647|string_max]
```

## Supported Data type
* bool
* int16, int32,int64
* varchar
* float, double
* date, datetime

## Supported SQL
* insert into [table] values ...
* insert into [table] (select ...)
* deletel from [table] [where ...]
* select [columns] from [table] [where ...] [group by ...] [having ...] [order by ...] [limit count, offset]
* select [columns] from (select ...) [where ...] [group by ...] [having ...] [order by ...] [limit num offset num]
