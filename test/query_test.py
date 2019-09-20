import sqlite3
import psycopg2
from random import randint


conn = sqlite3.connect("reference.db")
myconn = psycopg2.connect(database="test", user="", password="", host="127.0.0.1", port="5433")

cur = conn.cursor()
mycur = myconn.cursor()

cur.execute('DROP TABLE IF EXISTS querytest')
cur.execute('CREATE TABLE querytest (c1 INTEGER, c2 INTEGER, c3 INTEGER, c4 INTEGER, c5 INTEGER, PRIMARY KEY(c1,c2,c3))');

mycur.execute("delete from querytest")

for i in range(1000):
	insert_sql = 'INSERT INTO querytest VALUES({},{},{},{},{})'.format(
		randint(0, 1000),
		randint(0, 1000),
		randint(0, 1000),
		randint(0, 1000),
		randint(0, 1000))
	cur.execute(insert_sql);
	mycur.execute(insert_sql);
conn.commit()

def check(a, b):
	if len(a) != len(b):
		return False
	for j in range(len(a)):
		if type(a[j]) == float:
			if abs(a[j] - b[j]) > 0.0001:
				return False
		else:
			if a[j] != b[j]:
				return False
	return True
			
def testQuery(query_sql):
	cur.execute(query_sql)
	mycur.execute(query_sql)
	all_rows = cur.fetchall()
	my_all_rows = mycur.fetchall()
	assert(len(all_rows) == len(my_all_rows))
	for i in range(len(all_rows)):
		if not check(all_rows[i], my_all_rows[i]):
			print(all_rows[i])
			print(my_all_rows[i])
			assert 0

testQuery("select * from querytest order by c1,c2,c3,c4,c5")

for i in range(100):
	op = ["sum", "avg", "min", "max"]
	
	groupby = set()
	for i in range(randint(1, 5)):
		column = "c{}".format(randint(1, 5))
		groupby.add(column)
		
	projection = set()
	for i in range(randint(1, 5)):
		column = "c{}".format(randint(1, 5))
		func = op[randint(0, len(op) - 1)];
		if column in groupby:
			projection.add(column)
		else:
			projection.add("{}({})".format(func, column))
			
	groupby = ",".join(groupby);
	projection = ",".join(projection);
	query_sql = 'SELECT {} FROM querytest group by {} order by {}'.format(projection, groupby, groupby)
	print(query_sql)
	testQuery(query_sql)
	
for i in range(100):
	predicate = ""
	op = ["=", ">", "<", ">=", "<=", "!="]
	op2 = ["and", "or"]
	for i in range(randint(1, 5)):
		if predicate != "":
			predicate += " and " if randint(0, 1) == 0 else " or "
		predicate += "c{} {} {}".format(randint(1, 5), op[randint(0,len(op) - 1)], randint(0, 1000))
	query_sql = 'SELECT * FROM querytest where {} order by c1,c2,c3,c4,c5'.format(predicate)
	print(query_sql)
	testQuery(query_sql)
	

	
conn.close()
