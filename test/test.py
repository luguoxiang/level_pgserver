import psycopg2
import datetime
import time
import struct
con = psycopg2.connect(database="test", user="", password="", host="127.0.0.1", port="5433")

print("Database opened successfully")

cur = con.cursor()

cur.execute("delete from alltypes")
cur.execute("INSERT INTO alltypes VALUES (true, 123,123456,123456789, \"a\\nb\\tc\\123d\\x1ae\",7.89,12.3456,D'2019-09-09 11:12:13', D'1979-10-10')")

con.commit()
print("Record inserted successfully")

cur = con.cursor()
cur.execute("SELECT a,b,c,d,e,f,g,h,i,_rowkey from alltypes")
rows = cur.fetchall()

for row in rows:
    assert row[0] 
    assert row[1] == 123
    assert row[2] == 123456
    assert row[3] == 123456789
    assert row[4] == "a\nb\tc\123d\x1ae"
    assert row[5] == 7.89
    assert row[6] == 12.3456
    assert row[7] == datetime.datetime(2019,9,9,11,12,13)
    assert row[8] == datetime.date(1979, 10, 10)
    a,c,l, e,g,h = (struct.unpack("<?lH9sdq", bytes(row[9])))
    assert a
    assert c == 123456
    assert e == b'a\nb\tc\123d\x1ae'
    assert g == 12.3456
    assert row[7] == (datetime.datetime(*time.gmtime(h)[:6]))

print("Operation done successfully")
con.close()
