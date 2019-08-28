#include <ctime>
#include <iostream>
using namespace std;
int main()
{
	time_t curr_time;
	tm * curr_tm;
	char date_string[100];
	char time_string[100];
	
	time(&curr_time);
	curr_tm = localtime(&curr_time);
	
	cout<<strftime(date_string, 100, "%Y-%m-%d %H:%M:%S", curr_tm)<<endl;
	cout<<date_string<<endl;
	
	return 0;
}
