#include <iostream>
#include <map>

using namespace std;


//用insert函数插入数据，在数据的 插入上涉及到集合的唯一性这个概念，即当map中有这个关键字时，insert操作是插入数据不了的
//用数组方式可以覆盖以前该关键字对应的值

typedef struct MyData
{
	int x;
	int y;
}myDta;

//全局表读取



int main()
{
	map<int, string> map1;
	map<int, MyData> map2;
	
	map1[0] = "00000";
	map1[5] = "55555";
	map1.insert(map<int, string>::value_type(3, "33333"));
	map1.insert(pair<int, string>(7, "77777"));
	//map1.insert(make_pair<int, string>(7, "77777"));
	
	string str5 = map1[5];
	cout << "str5: " << str5 << endl;
	
	for(map<int,string>::iterator iter = map1.begin();iter!=map1.end();iter++)
    {
       int keyk = iter->first;
       string value = iter->second;
       cout << "value: " << value << endl;
    }
    
    cout << map1.erase(3) << endl;
    cout << map1[10] << endl;
    

	/*
	for(int i = 1; i <= map1.size(); i++)
    {
       cout << "value: " << map1[i] << endl;
    }
    */
    
    
    map2[2].x = 5;
    map2[2].y = 10;
    
    map2[1].x = 6;
    map2[1].y = 11;
    
    cout << map2[2].x << map2[2].y << endl;
    cout << map2[1].x << map2[1].y << endl;
	

	return 0;
}

