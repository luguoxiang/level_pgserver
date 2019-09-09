import psycopg2
from datetime import datetime
import struct
con = psycopg2.connect(database="test", user="", password="", host="127.0.0.1", port="5433")

print("Database opened successfully")

cur = con.cursor()

cur.execute("delete from test")
cur.execute("INSERT INTO test VALUES (123,\"a\\nb\\tc\\123d\\x1ae\",7.89,D'2019-09-09 11:12:13')")

con.commit()
print("Record inserted successfully")

cur = con.cursor()
cur.execute("SELECT a,b,c,d,_rowkey from test")
rows = cur.fetchall()

for row in rows:
    assert row[0] == 123
    assert row[1] == "a\nb\tc\123d\x1ae"
    assert row[2] == 7.89
    assert row[3] == datetime.strptime('2019-09-09 11:12:13', '%Y-%m-%d %H:%M:%S')
    print(len(row[4]))
    a,l, s = (struct.unpack("<lH9s", bytes(row[4])))
    assert a == 123
    assert l == 9
    assert s == b'a\nb\tc\123d\x1ae'

print("Operation done successfully")
con.close()
