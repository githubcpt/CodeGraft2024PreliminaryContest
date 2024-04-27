#pragma once

#define Scheme 1

#if defined(_WIN32)
#    include <math.h>
#    include <chrono>
#    include <iostream>
#    include <queue>
#    include <stack>
#    include <string>
#    include <unordered_map>
#    include <unordered_set>
#    include <vector>
#else
#    include <bits/stdc++.h>
#endif

enum CmdType
{
    // 机器人
    Move = 0,
    Get,
    Pull,
    // 轮船
    Rot,
    Ship,
    Berthing,
    Dept,
    // 买船
    Lboat,
    // 买机器人
    Lbot
};

struct OutputCmd
{
    CmdType cmd;
    int para1;
    int para2;
    int para3;

    OutputCmd(CmdType _cmd, int _para1, int _para2, int _para3) : para1(_para1), cmd(_cmd), para2(_para2), para3(_para3)
    {
    }
    OutputCmd(CmdType _cmd, int _para1, int _para2) : para1(_para1), cmd(_cmd), para2(_para2)
    {
    }
    OutputCmd(CmdType _cmd, int _para1) : cmd(_cmd), para1(_para1)
    {
    }
};

struct Point
{
    int x;
    int y;

    int cost;
    int heuristic;

    Point()
    {
        x = -1;
        y = -1;
    }
    Point(const int _x, const int _y) : x(_x), y(_y)
    {
    }
    Point(const int _x, const int _y, int _cost, int _heuristic) : x(_x), y(_y), cost(_cost), heuristic(_heuristic)
    {
    }
    bool operator==(const Point other)
    {
        return this->x == other.x && this->y == other.y;
    }
    bool operator!=(const Point other)
    {
        return this->x != other.x || this->y != other.y;
    }

    double norm()
    {
        return sqrt(x * x + y * y);
    }
    int diatance(const Point p)
    {
        return abs(x - p.x) + abs(y - p.y);
    }
    double sqrtDistance(const Point p)
    {
        return std::sqrt((double)this->diatance(p));
    }
    Point normalize()
    {
        return Point(x / norm(), y / norm());
    }
    Point operator+(const Point p)
    {
        return Point(x + p.x, y + p.y);
    }
    Point operator-(const Point p)
    {
        return Point(x - p.x, y - p.y);
    }
};

struct PointHash
{
    std::size_t operator()(const Point& pt) const
    {
        return std::hash<int>{}(pt.x) ^ (std::hash<int>{}(pt.y) << 1);
    }
};
struct PointEqual
{
    bool operator()(const Point& p1, const Point& p2) const
    {
        return p1.x == p2.x && p1.y == p2.y;
    }
};

struct PurchasePoint
{
    Point pt;
    std::vector<std::vector<int>> dist;

    PurchasePoint(Point _pt) : pt(_pt)
    {
    }
};

struct DeliveryPoint
{
    Point pt;
    std::vector<std::vector<std::vector<int>>> path;
    std::vector<std::vector<std::vector<int>>> dist;

    DeliveryPoint(Point _pt) : pt(_pt)
    {
    }
};

struct GoodInfo
{
    Point pt;
    int val;
    int startFrame;
    int restFrame;
    bool isOwn = false;
    int closestBerthIdx;

    std::vector<std::vector<int>> path;
    std::vector<std::vector<int>> dist;

    GoodInfo()
    {
    }
};

struct Box
{
    Point minB;
    Point maxB;

    Box(Point _minB, Point _maxB) : minB(_minB), maxB(_maxB)
    {
    }
};

struct Robot
{
    Point pt;
    int have_goods;
    int tarBerthIdx = -1; // 目的泊位索引
    int tarGoodIdx = -1;  // 目的货物索引

#if Scheme
    GoodInfo goods;
    int getGoods(GoodInfo& g)
    {
        goods = g;
        have_goods = 1;
        tarBerthIdx = g.closestBerthIdx;
        tarGoodIdx = -1;
        return 1;
    }
    GoodInfo pullGoods()
    {
        have_goods = 0;
        tarBerthIdx = -1;
        return goods;
    }
#else
    std::stack<GoodInfo> goodss;
    int getGoods(GoodInfo& g)
    {
        goodss.push(g);
        ++have_goods;
        tarBerthIdx = g.closestBerthIdx;
        tarGoodIdx = -1;
        return 1;
    }
    std::stack<GoodInfo> pullGoods()
    {
        auto gss = goodss;
        std::stack<GoodInfo>().swap(goodss);
        have_goods = 0;
        tarBerthIdx = -1;
        return gss;
    }
#endif // Scheme

    Robot()
    {
    }
};

struct Berth
{
    Point pt;
    int loading_speed;

    bool isOwn = false;
    int closedDPDis;
    int closedDPIdx;

    std::vector<std::vector<std::vector<int>>> berthingAreaPath;
    std::vector<std::vector<std::vector<int>>> berthingAreaDist;

    std::vector<std::vector<int>> path;
    std::vector<std::vector<int>> dist;

    int totalGoodsNum = 0; // 从开局到现在总共有过几个货物
    int totalGoodsVal = 0; // 从开局到现在总共有过多少价值
    int goodsNum = 0;
    int goodsVal = 0;
    std::queue<GoodInfo> goods;

#if Scheme
    int getGoods(GoodInfo g)
    {
        goods.push(g);
        goodsVal += g.val;
        ++totalGoodsNum;
        totalGoodsVal += g.val;
        return ++goodsNum;
    }
    int pullGoods(int num)
    {
        int val = 0;
        GoodInfo g;
        for (int i = 0; i < num; ++i)
        {
            g = goods.front();
            goods.pop();
            val += g.val;
        }
        goodsNum -= num;
        goodsVal -= val;
        return val;
    }
#else
    void getGoods(std::stack<GoodInfo> goodss)
    {
        while (!goodss.empty())
        {
            auto g = goodss.top();
            goodss.pop();
            goods.push(g);
            goodsVal += g.val;
            ++goodsNum;
            ++totalGoodsNum;
            totalGoodsVal += g.val;
        }
    }
    int pullGoods(int num)
    {
        int val = 0;
        GoodInfo g;
        for (int i = 0; i < num; ++i)
        {
            g = goods.front();
            goods.pop();
            val += g.val;
        }
        goodsNum -= num;
        goodsVal -= val;
        return val;
    }
#endif // Scheme

    Berth()
    {
    }
};

struct Boat
{
    Point pt;
    int dir;
    int goodsNum;
    int status;

    std::pair<bool, int> target; // first : true泊位 false交货点   second : 索引

    Boat()
    {
    }
};

struct Opera
{
    int op;
    int dist;
    Point np;
    int nd;
    Opera(int _op, int _dist, Point _np, int _nd) : op(_op), dist(_dist), np(_np), nd(_nd)
    {
    }
};

struct CmpOpera
{
    bool operator()(const Opera& a, const Opera& b) const
    {
        return a.dist < b.dist;
    }
};