import psycopg2
con = psycopg2.connect(database="test", user="", password="", host="127.0.0.1", port="5433")

print("Database opened successfully")

cur = con.cursor()

cur.execute("INSERT INTO test VALUES (123,456,7.89)")

con.commit()
print("Record inserted successfully")

cur = con.cursor()
cur.execute("SELECT * from test")
rows = cur.fetchall()

for row in rows:
    print("a =", row[0])
    print("b =", row[1])
    print("c =", row[2])
    print()

print("Operation done successfully")
con.close()
