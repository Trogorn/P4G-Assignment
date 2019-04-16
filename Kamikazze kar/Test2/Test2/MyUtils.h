#ifndef MYUTILSH
#define MYUTILSH
class MyUtils
{
public:
	MyUtils();
	~MyUtils();
	static float Deg2Rad(float deg);
	static float Rad2Deg(float rad);

private:
	static float deg2rad;
	static float rad2deg;
};

#endif