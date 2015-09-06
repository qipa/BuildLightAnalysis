#include "stdafx.h"
#include "MathUtility.h"
#include <algorithm>

string CStringToString(CString& str)
{
	CStringA stra(str.GetBuffer(0));
	str.ReleaseBuffer();
	string strString = stra.GetBuffer(0);
	stra.ReleaseBuffer();
	return strString;
};

CString stringToCString(string& str)
{
	CStringA ss;
	ss.Format("%s",str.c_str());
	CString ws(ss.GetBuffer(0));
	ss.ReleaseBuffer();
	return ws;
};


double calIntersectPointsOfLines(sLine& srcLine, sLine& dstLine)
{
	Vec2d d1 = srcLine.e - srcLine.s;
	Vec2d d2 = dstLine.e - dstLine.s;
	Vec2d a1 = srcLine.s;
	Vec2d a2 = dstLine.s;

	double D = d2.y * d1.x - d2.x * d1.y;
	if (D == 0.0)//平行
	{
		return -1.0;
	}
	double t1 = ((a1.y - a2.y) * d2.x - (a1.x - a2.x) * d2.y)/D;
	double t2 = ((a1.y - a2.y) * d1.x - (a1.x - a2.x) * d1.y)/D;

	if (t1 > 0 && t1 < 1 && fabs(t1) > 1e-6 && fabs(t1-1) > 1e-6 && (t2 > 0 && t2 < 1 || fabs(t2) < 1e-6 || fabs(t2-1) < 1e-6))
	{
		return t1;
	}
	else
	{
		return -1.0;
	}
}

//分割线段，按照交点分割
void SegmentLine(vector<sLine>& slines, vector<sLine>& outSlines)
{	
	for (int i = 0; i < slines.size(); i++)
	{
		vector<double> ts;
		for (int j = 0; j < slines.size(); j++)
		{
			if (j != i)
			{
				double t;
				t = calIntersectPointsOfLines(slines[i],slines[j]);
				if (t > 0)
				{
					ts.push_back(t);
				}
			}
		}
		std::sort(ts.begin(), ts.end());
		sLine::W_TYPE type_ = slines[i].type;
		if (ts.empty())
		{
			outSlines.push_back(slines[i]);
		}
		else
		{
			sLine line = slines[i];
			Vec2d dir = line.e - line.s;
			Vec2d p1 = line.s;
			Vec2d p2;
			for (int k = 0; k < ts.size(); k++)
			{
				p2 = line.s + ts[k] * dir;
				outSlines.push_back(sLine(p1, p2 ,type_));
				p1 = p2;
			}
			outSlines.push_back(sLine(p1, line.e, type_));
		}
	}
}


void calLenOfLines(sLine& srcLine, sLine& dstLine, double& startW, double& endW)
{
	Vec2d d1 = srcLine.e - srcLine.s;
	Vec2d d2 = dstLine.e - dstLine.s;
	Vec2d a1 = srcLine.s;
	Vec2d a2 = dstLine.s;

	double D = d2.y * d1.x - d2.x * d1.y;
	if (D == 0.0)//平行
	{
		startW = endW = 1.0e8;
		return;
	}
	double t1 = ((a1.y - a2.y) * d2.x - (a1.x - a2.x) * d2.y)/D;
	double t2 = ((a1.y - a2.y) * d1.x - (a1.x - a2.x) * d1.y)/D;
	
	if (t2 > 1 || t2 < 0)//代价要增加line2的修改
	{
		//line1代价
		double d1Length = d1.Length();
		if (t1 >= 0 && t1 <= 1)
		{
			startW = d1Length * t1;
			endW = d1Length * (1-t1);
		}
		else if (t1 < 0)
		{
			startW = -t1 * d1Length;
			endW = startW + d1Length;
		}
		else
		{
			endW = (t1 - 1 ) * d1Length;
			startW = endW + d1Length;
		}
		//line2代价
		double w2 = 1.0e8;
		if (t2 < 0)
		{
			w2 =  -t2 * d2.Length();
		}
		else
		{
			w2 =  (t2 - 1 ) * d2.Length();
		}
		startW += w2;
		endW += w2;
		return;
	}
	else//只需要计算line1的修改代价
	{
		double d1Length = d1.Length();
		if (t1 >= 0 && t1 <= 1)
		{
			startW = d1Length * t1;
			endW = d1Length * (1-t1);
			return;
		}
		else if (t1 < 0)
		{
			startW = -t1 * d1Length;
			endW = startW + d1Length;
			return;
		}
		else
		{
			endW = (t1 - 1 ) * d1Length;
			startW = endW + d1Length;
			return;
		}
	}
	return;
}
void OptimizeTwoLines(sLine& srcLine, sLine& dstLine, bool isStartP)
{
	Vec2d d1 = srcLine.e - srcLine.s;
	Vec2d d2 = dstLine.e - dstLine.s;
	Vec2d a1 = srcLine.s;
	Vec2d a2 = dstLine.s;

	double D = d2.y * d1.x - d2.x * d1.y;
	if (D == 0.0)//平行
	{
		return;
	}
	double t1 = ((a1.y - a2.y) * d2.x - (a1.x - a2.x) * d2.y)/D;
	double t2 = ((a1.y - a2.y) * d1.x - (a1.x - a2.x) * d1.y)/D;

	Vec2d intersectP = srcLine.s + d1 * t1;

	if (isStartP)
	{
		srcLine.s = intersectP;
	}
	else
	{
		srcLine.e = intersectP;
	}
	return;
}
//优化线段，去掉和补充
void OptimizeLine(vector<sLine>& slines, vector<sLine>& outSlines, double thW)
{
	vector<sLine> optimizeSlines = slines;
	for (int i = 0; i < slines.size(); i++)
	{
		double minStartW = 1.0e8, minEndW = 1.0e8;
		int minStartI = 0, minEndI = 0;
		for (int j = 0; j < slines.size(); j++)
		{
			if (j != i)
			{
				double startW, endW;
				calLenOfLines(slines[i],slines[j], startW, endW);
				if (startW < minStartW)
				{
					minStartW = startW;
					minStartI = j;
				}
				if (endW < minEndW)
				{
					minEndW = endW;
					minEndI = j;
				}
			}
		}
		if (minStartW < thW)//有一个需要修改的
		{
			OptimizeTwoLines(optimizeSlines[i], slines[minStartI], true);
		}
		if (minEndW < thW)//有一个需要修改的
		{
			OptimizeTwoLines(optimizeSlines[i], slines[minEndI], false);
		}
	}
	SegmentLine(optimizeSlines, outSlines);
}

double lenOfLinePoint(sLine& line, Vec2d p)
{
	Vec2d d1 = line.e - line.s;
	Vec2d d2 = p - line.s;

	double l1 = d1.Length();
	double t = d1*d2 / l1;

	if (t >= 0 && t <= l1)
	{
		return fabs(d1/d2) / l1;
	}
	else if (t < 0)
	{
		return d2.Length();
	}
	else
	{
		return (p - line.e).Length();
	}

}
//相等的阈值，20mm
bool isEqual(Vec2d a, Vec2d b)
{
	double x = a.x - b.x;
	double y = a.y - b.y;
	return x*x + y*y < 400; //20 * 20
}
//求线段集合形成的封闭多边形，如果不封闭则返回false
bool CalClosedPolygon(list<sLine>& lines, vector<Vec2d>& outPoints)
{
	if (lines.size() < 3)
	{
		return false;
	}

	list<sLine>::iterator i;
	i = lines.begin();
	sLine line = *i;
	Vec2d p = line.s;
	lines.erase(i);
	outPoints.push_back(p);
	do
	{
		bool flag = false;
		for (list<sLine>::iterator j = lines.begin(); j != lines.end(); j++)
		{
			if (isEqual(p,(*j).s))
			{
				p = (*j).e;
				lines.erase(j);
				flag = true;
				outPoints.push_back(p);
				break;
			}
			if (isEqual(p,(*j).e))
			{
				p = (*j).s;
				lines.erase(j);
				flag = true;
				outPoints.push_back(p);
				break;
			}
		}

		if (!flag)
		{
			return false;
		}
	}
	while(!isEqual(p,line.e));

	return true;
}

//判断多边形是逆时针还是顺时针, true：逆时针
bool isAntiClock(vector<Vec2d>& polygon)
{
	int sz = polygon.size();
	int greaterC = 0, lessC = 0;
	for (int i = 0; i < polygon.size(); i++)
	{
		Vec2d a = polygon[i];
		Vec2d b = polygon[(i+1)%sz];
		Vec2d c = polygon[(i+2)%sz];
		if ((b-a)/(c-b) > 0)
			greaterC ++;
		else
			lessC ++;
	}
	if (greaterC > lessC)
	{
		return true;
	}
	else
		return false;
}