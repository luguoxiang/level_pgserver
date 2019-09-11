import psycopg2
import datetime
import struct
con = psycopg2.connect(database="test", user="", password="", host="127.0.0.1", port="5433")

print("Database opened successfully")

cur = con.cursor()

cur.execute("delete from alltypes")
cur.execute("INSERT INTO alltypes VALUES (123,\"a\\nb\\tc\\123d\\x1ae\",7.89,D'2019-09-09 11:12:13', D'1979-10-10')")

con.commit()
print("Record inserted successfully")

cur = con.cursor()
cur.execute("SELECT a,b,c,d,e,_rowkey from alltypes")
rows = cur.fetchall()

for row in rows:
    assert row[0] == 123
    assert row[1] == "a\nb\tc\123d\x1ae"
    assert row[2] == 7.89
    assert row[3] == datetime.datetime(2019,9,9,11,12,13)
    assert row[4] == datetime.date(1979, 10, 10)
    a,l, s = (struct.unpack("<lH9s", bytes(row[5])))
    assert a == 123
    assert l == 9
    assert s == b'a\nb\tc\123d\x1ae'

print("Operation done successfully")
con.close()
