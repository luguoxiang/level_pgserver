import psycopg2
import datetime
import time
import struct
con = psycopg2.connect(database="test", user="", password="", host="127.0.0.1", port="5433")

print("Database opened successfully")

COUNT=10000
with open("test.csv",'w') as f:
	f.write("a,b,c,d,e,f,g,h,i\n")
	for i in range (COUNT):
		f.write("%s,%d,%d,%d,%s,%f,%f,%s,%s\n" % ((i % 2) == 0, i + 123, i + 123456, i + 123456789, "\"\"abc\"',\"def", i + 7.89, i + 12.3456,'2019-09-09 11:12:13', '1979-10-10'))

cur = con.cursor()
cur.execute("SELECT * from csvtest")
rows = cur.fetchall()

print("validate")
for i in range (COUNT):
    row = rows[i];
    assert row[0] == ((i % 2) == 0)
    assert row[1] == 123 + i
    assert row[2] == 123456 + i
    assert row[3] == 123456789  + i 
    assert row[4] == '"abc\',def'
    assert abs(row[5] - 7.89 - i) < 0.0001
    assert abs(row[6] - 12.3456 - i) < 0.0001
    assert row[7] == datetime.datetime(2019,9,9,11,12,13)
    assert row[8] == datetime.date(1979, 10, 10)


cur.execute("delete from alltypes")
sql="insert into alltypes select * from csvtest"
print(sql)
cur.execute(sql)
con.commit()

print("validate")
cur.execute("SELECT * from alltypes order by b")
rows = cur.fetchall()

for i in range (COUNT):
    row = rows[i];
    assert row[0] == ((i % 2) == 0)
    assert row[1] == 123 + i
    assert row[2] == 123456 + i
    assert row[3] == 123456789  + i 
    assert row[4] == '"abc\',def'
    assert abs(row[5] - 7.89 - i) < 0.001
    assert abs(row[6] - 12.3456 - i) < 0.001
    assert row[7] == datetime.datetime(2019,9,9,11,12,13)
    assert row[8] == datetime.date(1979, 10, 10)
print("Operation done successfully")
con.close()
