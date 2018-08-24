#ifndef RES_H
#define RES_H

#include <vector>
using namespace std;

class Res
{
public:
	Res(double res, vector<unsigned int> ind, int dir);
	virtual ~Res();
	float getRes() { return result; };
	vector<unsigned int> getIndex() { return index; };
	int getDirection() { return direction; };
private:
	float result;
	vector<unsigned int> index; // eg: {1,2,3,4} means X1 cut1,x3 cut 4
	int direction;              // >=0 R; <0 L
};

#endif // RES_H
