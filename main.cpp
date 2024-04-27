#pragma once
#include "constants.h"
#include "object.h"
#include "util.h"

int boatMaxSize = 1;
int robotMaxSize = 16;
double parameter1 = 0.001;

// 船的容量、全局帧ID、全局金钱数、泊位、机器人、船数量
int boat_capacity, globalFrameID, globalMoney, berthNum, robotNum, boatNum;
// 地图数据
char map[Constancts::N][Constancts::N];
// 泊位、机器人、船
std::vector<Berth> berth;
std::vector<Robot> robot(30);
std::vector<Boat> boat(10);
// 货物全局索引
int globalGoodsIdx = 0;
// 所有存在的货物
std::unordered_map<int, GoodInfo> allGoods;
// 标记地图上某个位置对应的货物索引 -1表示没有货物
std::vector<std::vector<int>> goodsMap(Constancts::N, std::vector<int>(Constancts::N, -1));
// 标记机器人可以走的位置 0不能走 1一般空地 2特殊地块
std::vector<std::vector<int>> robotMap(Constancts::N, std::vector<int>(Constancts::N, 0));
// 标记轮船可以走的位置 0不能走 1一般空地 2特殊地块
std::vector<std::vector<int>> boatMap(Constancts::N, std::vector<int>(Constancts::N, 0));
// 购买点
std::vector<PurchasePoint> robotPurchasePt, boatPurchasePt;
std::vector<DeliveryPoint> deliveryPt;

std::unordered_map<int, std::unordered_map<int, int>> berthPosToIdx;
// 给货物预处理加速判断
std::vector<std::vector<bool>> speedUpMap(Constancts::N + 2 * Constancts::D1,
                                          std::vector<bool>(Constancts::N + 2 * Constancts::D1, false));
// 机器人最大货物
std::unordered_map<int, int> robotMaxGoods;

void Init()
{
    for (int i = 0; i < Constancts::N; ++i)
    {
        scanf("%s", map[i]);
    }

    scanf("%d", &berthNum);
    berth.resize(berthNum);
    for (int i = 0; i < berthNum; ++i)
    {
        int id;
        scanf("%d%d%d%d", &id, &berth[i].pt.x, &berth[i].pt.y, &berth[i].loading_speed);
        berthPosToIdx[berth[i].pt.x][berth[i].pt.y] = id;
        berth[i].isOwn = false;
    }
    scanf("%d", &boat_capacity);

    for (int i = 0; i < speedUpMap.size(); ++i)
    {
        for (int j = 0; j < speedUpMap[i].size(); ++j)
        {
            if (Util::isValid(Point{i - Constancts::D1, j - Constancts::D1}))
            {
                speedUpMap[i][j] = true;
            }
        }
    }

    for (int i = 0; i < boat.size(); ++i)
    {
        boat[i].target = {false, -1};
    }
    Util::preprocess(map, berthNum, berth, robotMap, boatMap, robotPurchasePt, boatPurchasePt, deliveryPt);

    if (robotPurchasePt[0].pt == Point{101, 38}) // 1
    {
        robotMaxSize = 11;
        // parameter1 = 0.4;
        boatMaxSize = 2;
    }
    else if (robotPurchasePt[0].pt == Point{46, 46}) // 2
    {
        robotMaxSize = 15;
        // parameter1 = 0.1;
        boatMaxSize = 1;
    }
    else // 3
    {
        robotMaxSize = 17;
        // parameter1 = 0.1;
        boatMaxSize = 1;
    }

    char okk[100];
    scanf("%s", okk);
    printf("OK\n");
    fflush(stdout);
}

void Input()
{
    scanf("%d", &globalMoney);

    // 新增货物数量
    int num;
    scanf("%d", &num);
    for (int i = 1; i <= num; ++i)
    {
        int x, y, val;
        scanf("%d%d%d", &x, &y, &val);
        if (val == 0) // 货物价格为0
            continue;
        int minIdx = -1;
        int minDis = INT_MAX;
        for (int j = 0; j < berthNum; ++j)
        {
            // 轮船可达 && 机器人可达
            if (berth[j].dist[x][y] >= 0 && berth[j].dist[x][y] < minDis)
            {
                minDis = berth[j].dist[x][y];
                minIdx = j;
            }
        }
        if (minIdx != -1) // 货物可达 才放入
        {
            allGoods[globalGoodsIdx] = GoodInfo();
            allGoods[globalGoodsIdx].pt = {x, y};
            allGoods[globalGoodsIdx].val = val;
            allGoods[globalGoodsIdx].startFrame = globalFrameID;
            allGoods[globalGoodsIdx].closestBerthIdx = minIdx;
            Util::preprocessGoods(robotMap, speedUpMap, allGoods[globalGoodsIdx]);
            goodsMap[x][y] = globalGoodsIdx;
            ++globalGoodsIdx;
        }
    }

    scanf("%d", &robotNum);
    for (int i = 0; i < robotNum; ++i)
    {
        int id;
        scanf("%d", &id);
        scanf("%d%d%d", &robot[id].have_goods, &robot[id].pt.x, &robot[id].pt.y);
    }

    scanf("%d", &boatNum);
    for (int i = 0; i < boatNum; ++i)
    {
        int id;
        scanf("%d", &id);
        scanf("%d%d%d%d%d\n", &boat[id].goodsNum, &boat[id].pt.x, &boat[id].pt.y, &boat[id].dir, &boat[id].status);
    }
    char okk[100];
    scanf("%s", okk);
}

void printComands(std::vector<OutputCmd> outputCmds)
{
    for (int i = 0; i < outputCmds.size(); ++i)
    {
        if (outputCmds[i].cmd == CmdType::Lbot)
        {
            printf("%s %d %d %d\n",
                   Constancts::cmdToChar[outputCmds[i].cmd],
                   outputCmds[i].para1,
                   outputCmds[i].para2,
                   outputCmds[i].para3);
        }
        else if (outputCmds[i].cmd == CmdType::Move || outputCmds[i].cmd == CmdType::Lboat ||
                 outputCmds[i].cmd == CmdType::Rot)
        {
            printf("%s %d %d\n", Constancts::cmdToChar[outputCmds[i].cmd], outputCmds[i].para1, outputCmds[i].para2);
        }
        else
        {
            printf("%s %d\n", Constancts::cmdToChar[outputCmds[i].cmd], outputCmds[i].para1);
        }
    }
}

void buyRobot(std::vector<OutputCmd>& outputCmds)
{
    if (robotNum >= robotMaxSize || (boatNum == 0 && (globalMoney - Constancts::robotPrice < Constancts::boatPrice)) ||
        (boatNum && (globalMoney < Constancts::robotPrice)))
    {
        return;
    }

    auto curMoney = globalMoney;
    auto curRobotNum = robotNum;
    std::vector<std::pair<int, int>> distToPP(robotPurchasePt.size());
    for (int i = 0; i < robotPurchasePt.size(); ++i)
    {
        distToPP[i].first = i;
        distToPP[i].second = 0;
        for (auto& [_, goods] : allGoods)
        {
            distToPP[i].second += goods.dist[robotPurchasePt[i].pt.x][robotPurchasePt[i].pt.y];
        }
    }
    std::sort(distToPP.begin(), distToPP.end(), [](std::pair<int, int> a, std::pair<int, int> b) {
        return a.second < b.second;
    });
    for (int i = 0; i < distToPP.size(); ++i)
    {
        if (curRobotNum >= robotMaxSize ||
            (boatNum == 0 && (curMoney - Constancts::robotPrice < Constancts::boatPrice)) ||
            (boatNum && (curMoney < Constancts::robotPrice)))
        {
            return;
        }
        outputCmds.push_back(
            {CmdType::Lbot, robotPurchasePt[distToPP[i].first].pt.x, robotPurchasePt[distToPP[i].first].pt.y, 0});
        robotMaxGoods[curRobotNum] = 1;
        curMoney -= Constancts::robotPrice;
        ++curRobotNum;
    }
}

void buyBoat(std::vector<OutputCmd>& outputCmds)
{
    if (boatNum >= boatMaxSize || globalMoney < Constancts::boatPrice)
        return;

    int maxVal = 0;
    int maxIdx = -1;
    for (int i = 0; i < berthNum; ++i)
    {
        if (berth[i].goodsVal > /*=*/maxVal)
        {
            maxVal = berth[i].goodsVal;
            maxIdx = i;
        }
    }
    if (maxVal > /*=*/0)
    {
        int minIdx = -1;
        int minDis = INT_MAX;
        for (int i = 0; i < boatPurchasePt.size(); ++i)
        {
            if (boatPurchasePt[i].dist[berth[maxIdx].pt.x][berth[maxIdx].pt.y] >= 0 &&
                boatPurchasePt[i].dist[berth[maxIdx].pt.x][berth[maxIdx].pt.y] < minDis)
            {
                minDis = boatPurchasePt[i].dist[berth[maxIdx].pt.x][berth[maxIdx].pt.y];
                minIdx = i;
            }
        }
        outputCmds.push_back({CmdType::Lboat, boatPurchasePt[minIdx].pt.x, boatPurchasePt[minIdx].pt.y});
    }
}

void robotGetAndPull(std::vector<OutputCmd>& outputCmds)
{
    for (int i = 0; i < robotNum; ++i)
    {
        if (robot[i].have_goods) // 可以运行且有货物
        {
            auto berthIdx = robot[i].tarBerthIdx;
            // 如果已经在目的泊位范围内
            if (berthIdx != -1 && berth[berthIdx].path[robot[i].pt.x][robot[i].pt.y] == -2)
            {
                // 机器人放下货物 泊位接受货物
                outputCmds.push_back({CmdType::Pull, i});
                berth[berthIdx].getGoods(robot[i].pullGoods());
            }
        }
        else // 可以运行且没有货物
        {
            auto tarGoodIdx = robot[i].tarGoodIdx;
            // 存在预约货物 && 预约货物没消失 && 在货物范围内
            if (tarGoodIdx != -1 && allGoods.count(tarGoodIdx) && allGoods[tarGoodIdx].pt == robot[i].pt)
            {
                outputCmds.push_back({CmdType::Get, i});
                robot[i].getGoods(allGoods[tarGoodIdx]);
                goodsMap[allGoods[tarGoodIdx].pt.x][allGoods[tarGoodIdx].pt.y] = -1;
                allGoods.erase(tarGoodIdx);
            }
        }
    }
}

void robotGetAndPull1(std::vector<OutputCmd>& outputCmds)
{
    for (int i = 0; i < robotNum; ++i)
    {
        auto berthIdx = robot[i].tarBerthIdx;
        // 如果已经在目的泊位范围内
        if (berthIdx != -1 && berth[berthIdx].path[robot[i].pt.x][robot[i].pt.y] == -2)
        {
            // 机器人放下货物 泊位接受货物
            outputCmds.push_back({CmdType::Pull, i});
            berth[berthIdx].getGoods(robot[i].pullGoods());
        }

        auto tarGoodIdx = robot[i].tarGoodIdx;
        // 存在预约货物 && 预约货物没消失 && 在货物范围内
        if (tarGoodIdx != -1 && allGoods.count(tarGoodIdx) && allGoods[tarGoodIdx].pt == robot[i].pt)
        {
            outputCmds.push_back({CmdType::Get, i});
            robot[i].getGoods(allGoods[tarGoodIdx]);
            goodsMap[allGoods[tarGoodIdx].pt.x][allGoods[tarGoodIdx].pt.y] = -1;
            allGoods.erase(tarGoodIdx);
        }
    }
}

void getBestGoods(int ridx)
{
    auto rpt = robot[ridx].pt;
    int maxVal = -1;
    int goodIdx = -1;
    for (auto& [idx, goods] : allGoods)
    {
        // 没有路径 || 已被预订 || 到了货物就消失了(预留5帧的碰撞时间)
        if (goods.isOwn || goods.dist[rpt.x][rpt.y] < 0 || ((goods.dist[rpt.x][rpt.y] + 5) >= goods.restFrame))
        {
            continue;
        }
        auto dist = goods.dist[rpt.x][rpt.y] /*+ berth[goods.closestBerthIdx].dist[goods.pt.x][goods.pt.y]*/;
        auto curVal = goods.val / (dist * parameter1);
        if (maxVal < curVal)
        {
            maxVal = curVal;
            goodIdx = idx;
        }
    }

    if (goodIdx != -1)
    {
        robot[ridx].tarGoodIdx = goodIdx;
        allGoods[goodIdx].isOwn = true;
    }
}

void getBestGoods1(int ridx)
{
    auto rpt = robot[ridx].pt;
    int maxVal = -1;
    int goodIdx = -1;
    for (auto& [idx, goods] : allGoods)
    {
        // 没有路径 || 已被预订 || 到了货物就消失了(预留5帧的碰撞时间)
        if (goods.isOwn || goods.dist[rpt.x][rpt.y] < 0 || ((goods.dist[rpt.x][rpt.y] + 5) >= goods.restFrame))
        {
            continue;
        }
        auto dist = goods.dist[rpt.x][rpt.y] /*+ berth[goods.closestBerthIdx].dist[goods.pt.x][goods.pt.y]*/;
        auto curVal = goods.val / (dist * parameter1);
        if (maxVal < curVal)
        {
            maxVal = curVal;
            goodIdx = idx;
        }
    }

    if (goodIdx != -1)
    {
        robot[ridx].tarGoodIdx = goodIdx;
        allGoods[goodIdx].isOwn = true;
    }
}

void updateBetterGoods(int ridx)
{
    auto rpt = robot[ridx].pt;
    auto curGoodsIdx = robot[ridx].tarGoodIdx;
    auto curGoodsDist = allGoods[curGoodsIdx].dist[rpt.x][rpt.y];
    auto curGoodsVal = allGoods[curGoodsIdx].val;
    int goodIdx = -1;
    for (auto& [idx, goods] : allGoods)
    {
        // 已被预订 || 没有路径 || 大于当前距离 || 到了货物就消失了(预留5帧的碰撞时间)
        if (goods.isOwn || goods.dist[rpt.x][rpt.y] < 0 || goods.dist[rpt.x][rpt.y] >= curGoodsDist ||
            ((goods.dist[rpt.x][rpt.y] + 5) >= goods.restFrame))
        {
            continue;
        }
        if (curGoodsVal < goods.val)
        {
            // curGoodsDist = goods.dist[rpt.x][rpt.y];
            curGoodsVal = goods.val;
            goodIdx = idx;
        }
    }

    if (goodIdx != -1)
    {
        robot[ridx].tarGoodIdx = goodIdx;
        allGoods[goodIdx].isOwn = true;
        allGoods[curGoodsIdx].isOwn = false;
    }
}

void addCmdIfInGood(Point nextPos, int ridx, std::vector<OutputCmd>& outputCmds)
{
    auto tarGoodIdx = robot[ridx].tarGoodIdx;
    // 到了目的货物位置
    if (allGoods[tarGoodIdx].pt == nextPos)
    {
        outputCmds.push_back({CmdType::Get, ridx});
        robot[ridx].getGoods(allGoods[tarGoodIdx]);
        allGoods.erase(tarGoodIdx);
        goodsMap[nextPos.x][nextPos.y] = -1;
    }
}

void addCmdIfInBerth(Point nextPos, int ridx, std::vector<OutputCmd>& outputCmds)
{
    auto berthIdx = robot[ridx].tarBerthIdx;
    // 如果已经在泊位范围内 机器人放下货物 泊位接受货物
    if (berthIdx != -1 && berth[berthIdx].path[nextPos.x][nextPos.y] == -2)
    {
        outputCmds.push_back({CmdType::Pull, ridx});
        berth[berthIdx].getGoods(robot[ridx].pullGoods());
    }
}

void robotMove(std::vector<OutputCmd>& outputCmds)
{
    auto tmpMap = robotMap;
    for (int i = 0; i < robotNum; ++i)
    {
        if (robotMap[robot[i].pt.x][robot[i].pt.y] == 1)
            tmpMap[robot[i].pt.x][robot[i].pt.y] = 0;
    }

    for (int i = 0; i < robotNum; ++i)
    {
        if (robot[i].have_goods)
        {
            auto tarBerth = robot[i].tarBerthIdx;
            if (tarBerth != -1) // 必不为-1
            {
                auto moveDir = berth[tarBerth].path[robot[i].pt.x][robot[i].pt.y];
                auto np = Util::calcNextPos(robot[i].pt, moveDir);
                if (tmpMap[np.x][np.y] != 0)
                {
                    outputCmds.push_back({CmdType::Move, i, moveDir});
                    tmpMap[robot[i].pt.x][robot[i].pt.y] = 1;
                    if (tmpMap[np.x][np.y] == 1)
                        tmpMap[np.x][np.y] = 0;
                    addCmdIfInBerth(np, i, outputCmds);
                }
                else
                {
                    std::vector<std::pair<int, std::pair<int, Point>>> idxDist;
                    for (int j = 0; j < 4; ++j)
                    {
                        if (j == moveDir)
                            continue;
                        np = Util::calcNextPos(robot[i].pt, j);
                        if (speedUpMap[np.x + Constancts::D1][np.y + Constancts::D1] &&
                            berth[tarBerth].dist[np.x][np.y] >= 0)
                        {
                            idxDist.push_back({j, {berth[tarBerth].dist[np.x][np.y], np}});
                        }
                    }
                    std::sort(idxDist.begin(),
                              idxDist.end(),
                              [](std::pair<int, std::pair<int, Point>> a, std::pair<int, std::pair<int, Point>> b) {
                                  return a.second.first < b.second.first;
                              });
                    for (int j = 0; j < idxDist.size(); ++j)
                    {
                        np = idxDist[j].second.second;
                        if (tmpMap[np.x][np.y] != 0)
                        {
                            outputCmds.push_back({CmdType::Move, i, idxDist[j].first});
                            if (tmpMap[robot[i].pt.x][robot[i].pt.y] == 0)
                                tmpMap[robot[i].pt.x][robot[i].pt.y] = 1;
                            if (tmpMap[np.x][np.y] == 1)
                                tmpMap[np.x][np.y] = 0;
                            addCmdIfInBerth(np, i, outputCmds);
                            break;
                        }
                    }
                }
            }
        }
        else
        {
            if (robot[i].tarGoodIdx != -1 && allGoods.count(robot[i].tarGoodIdx))
            {
                updateBetterGoods(i);
            }
            else // 初始状态 || 送完货物 || 货物消失
            {
                getBestGoods(i);
            }

            auto tarGoods = robot[i].tarGoodIdx;
            if (tarGoods != -1)
            {
                auto moveDir = allGoods[tarGoods].path[robot[i].pt.x][robot[i].pt.y];
                if (moveDir == -2) // 更新货物时 最好的货物在脚下
                {
                    addCmdIfInGood(robot[i].pt, i, outputCmds);
                    continue;
                }
                auto np = Util::calcNextPos(robot[i].pt, moveDir);
                if (tmpMap[np.x][np.y] != 0)
                {
                    outputCmds.push_back({CmdType::Move, i, moveDir});
                    tmpMap[robot[i].pt.x][robot[i].pt.y] = 1;
                    if (tmpMap[np.x][np.y] == 1)
                        tmpMap[np.x][np.y] = 0;
                    addCmdIfInGood(np, i, outputCmds);
                }
                else
                {
                    std::vector<std::pair<int, std::pair<int, Point>>> idxDist;
                    for (int j = 0; j < 4; ++j)
                    {
                        if (j == moveDir)
                            continue;
                        np = Util::calcNextPos(robot[i].pt, j);
                        if (speedUpMap[np.x + Constancts::D1][np.y + Constancts::D1] &&
                            allGoods[tarGoods].dist[np.x][np.y] >= 0)
                        {
                            idxDist.push_back({j, {allGoods[tarGoods].dist[np.x][np.y], np}});
                        }
                    }
                    std::sort(idxDist.begin(),
                              idxDist.end(),
                              [](std::pair<int, std::pair<int, Point>> a, std::pair<int, std::pair<int, Point>> b) {
                                  return a.second.first < b.second.first;
                              });
                    for (int j = 0; j < idxDist.size(); ++j)
                    {
                        np = idxDist[j].second.second;
                        if (tmpMap[np.x][np.y] != 0)
                        {
                            outputCmds.push_back({CmdType::Move, i, idxDist[j].first});
                            if (tmpMap[robot[i].pt.x][robot[i].pt.y] == 0)
                                tmpMap[robot[i].pt.x][robot[i].pt.y] = 1;
                            if (tmpMap[np.x][np.y] == 1)
                                tmpMap[np.x][np.y] = 0;
                            addCmdIfInGood(np, i, outputCmds);
                            break;
                        }
                    }
                }
            }
        }
    }
}

void robotMove1(std::vector<OutputCmd>& outputCmds)
{
    auto tmpMap = robotMap;
    for (int i = 0; i < robotNum; ++i)
    {
        if (robotMap[robot[i].pt.x][robot[i].pt.y] == 1)
            tmpMap[robot[i].pt.x][robot[i].pt.y] = 0;
    }

    for (int i = 0; i < robotNum; ++i)
    {
        if (robot[i].have_goods == 0)
        {
            if (robot[i].tarGoodIdx != -1 && allGoods.count(robot[i].tarGoodIdx))
            {
                updateBetterGoods(i);
            }
            else // 初始状态 || 送完货物 || 货物消失
            {
                getBestGoods(i);
            }

            auto tarGoods = robot[i].tarGoodIdx;
            if (tarGoods != -1)
            {
                auto moveDir = allGoods[tarGoods].path[robot[i].pt.x][robot[i].pt.y];
                if (moveDir == -2) // 更新货物时 最好的货物在脚下
                {
                    addCmdIfInGood(robot[i].pt, i, outputCmds);
                    continue;
                }
                auto np = Util::calcNextPos(robot[i].pt, moveDir);
                if (tmpMap[np.x][np.y] != 0)
                {
                    outputCmds.push_back({CmdType::Move, i, moveDir});
                    tmpMap[robot[i].pt.x][robot[i].pt.y] = 1;
                    if (tmpMap[np.x][np.y] == 1)
                        tmpMap[np.x][np.y] = 0;
                    addCmdIfInGood(np, i, outputCmds);
                }
                else
                {
                    std::vector<std::pair<int, std::pair<int, Point>>> idxDist;
                    for (int j = 0; j < 4; ++j)
                    {
                        if (j == moveDir)
                            continue;
                        np = Util::calcNextPos(robot[i].pt, j);
                        if (speedUpMap[np.x + Constancts::D1][np.y + Constancts::D1] &&
                            allGoods[tarGoods].dist[np.x][np.y] >= 0)
                        {
                            idxDist.push_back({j, {allGoods[tarGoods].dist[np.x][np.y], np}});
                        }
                    }
                    std::sort(idxDist.begin(),
                              idxDist.end(),
                              [](std::pair<int, std::pair<int, Point>> a, std::pair<int, std::pair<int, Point>> b) {
                                  return a.second.first < b.second.first;
                              });
                    for (int j = 0; j < idxDist.size(); ++j)
                    {
                        np = idxDist[j].second.second;
                        if (tmpMap[np.x][np.y] != 0)
                        {
                            outputCmds.push_back({CmdType::Move, i, idxDist[j].first});
                            if (tmpMap[robot[i].pt.x][robot[i].pt.y] == 0)
                                tmpMap[robot[i].pt.x][robot[i].pt.y] = 1;
                            if (tmpMap[np.x][np.y] == 1)
                                tmpMap[np.x][np.y] = 0;
                            addCmdIfInGood(np, i, outputCmds);
                            break;
                        }
                    }
                }
            }
        }
        else if (robot[i].have_goods == 1 && robot[i].have_goods == robotMaxGoods[i]) // 货满
        {
            auto tarBerth = robot[i].tarBerthIdx;
            if (tarBerth != -1) // 必不为-1
            {
                auto moveDir = berth[tarBerth].path[robot[i].pt.x][robot[i].pt.y];
                auto np = Util::calcNextPos(robot[i].pt, moveDir);
                if (tmpMap[np.x][np.y] != 0)
                {
                    outputCmds.push_back({CmdType::Move, i, moveDir});
                    tmpMap[robot[i].pt.x][robot[i].pt.y] = 1;
                    if (tmpMap[np.x][np.y] == 1)
                        tmpMap[np.x][np.y] = 0;
                    addCmdIfInBerth(np, i, outputCmds);
                }
                else
                {
                    std::vector<std::pair<int, std::pair<int, Point>>> idxDist;
                    for (int j = 0; j < 4; ++j)
                    {
                        if (j == moveDir)
                            continue;
                        np = Util::calcNextPos(robot[i].pt, j);
                        if (speedUpMap[np.x + Constancts::D1][np.y + Constancts::D1] &&
                            berth[tarBerth].dist[np.x][np.y] >= 0)
                        {
                            idxDist.push_back({j, {berth[tarBerth].dist[np.x][np.y], np}});
                        }
                    }
                    std::sort(idxDist.begin(),
                              idxDist.end(),
                              [](std::pair<int, std::pair<int, Point>> a, std::pair<int, std::pair<int, Point>> b) {
                                  return a.second.first < b.second.first;
                              });
                    for (int j = 0; j < idxDist.size(); ++j)
                    {
                        np = idxDist[j].second.second;
                        if (tmpMap[np.x][np.y] != 0)
                        {
                            outputCmds.push_back({CmdType::Move, i, idxDist[j].first});
                            if (tmpMap[robot[i].pt.x][robot[i].pt.y] == 0)
                                tmpMap[robot[i].pt.x][robot[i].pt.y] = 1;
                            if (tmpMap[np.x][np.y] == 1)
                                tmpMap[np.x][np.y] = 0;
                            addCmdIfInBerth(np, i, outputCmds);
                            break;
                        }
                    }
                }
            }
        }
        else if (robot[i].have_goods == 1 && robot[i].have_goods < robotMaxGoods[i]) // 货不满
        {
            bool isGoBerth = false;
            // 在范围内找一个距离比较近的货物
            if (robot[i].tarGoodIdx != -1 && allGoods.count(robot[i].tarGoodIdx))
            {
                updateBetterGoods(i);
            }
            else // 初始状态 || 送完货物 || 货物消失
            {
                auto tarBerth = robot[i].tarBerthIdx;
                auto distToBerth = (tarBerth == -1) ? INT_MAX : berth[tarBerth].dist[robot[i].pt.x][robot[i].pt.y];
                getBestGoods1(i);
            }

            auto tarGoods = robot[i].tarGoodIdx;
            if (tarGoods != -1)
            {
                auto moveDir = allGoods[tarGoods].path[robot[i].pt.x][robot[i].pt.y];
                if (moveDir == -2) // 更新货物时 最好的货物在脚下
                {
                    addCmdIfInGood(robot[i].pt, i, outputCmds);
                    continue;
                }
                auto np = Util::calcNextPos(robot[i].pt, moveDir);
                if (tmpMap[np.x][np.y] != 0)
                {
                    outputCmds.push_back({CmdType::Move, i, moveDir});
                    tmpMap[robot[i].pt.x][robot[i].pt.y] = 1;
                    if (tmpMap[np.x][np.y] == 1)
                        tmpMap[np.x][np.y] = 0;
                    addCmdIfInGood(np, i, outputCmds);
                }
                else
                {
                    std::vector<std::pair<int, std::pair<int, Point>>> idxDist;
                    for (int j = 0; j < 4; ++j)
                    {
                        if (j == moveDir)
                            continue;
                        np = Util::calcNextPos(robot[i].pt, j);
                        if (speedUpMap[np.x + Constancts::D1][np.y + Constancts::D1] &&
                            allGoods[tarGoods].dist[np.x][np.y] >= 0)
                        {
                            idxDist.push_back({j, {allGoods[tarGoods].dist[np.x][np.y], np}});
                        }
                    }
                    std::sort(idxDist.begin(),
                              idxDist.end(),
                              [](std::pair<int, std::pair<int, Point>> a, std::pair<int, std::pair<int, Point>> b) {
                                  return a.second.first < b.second.first;
                              });
                    for (int j = 0; j < idxDist.size(); ++j)
                    {
                        np = idxDist[j].second.second;
                        if (tmpMap[np.x][np.y] != 0)
                        {
                            outputCmds.push_back({CmdType::Move, i, idxDist[j].first});
                            if (tmpMap[robot[i].pt.x][robot[i].pt.y] == 0)
                                tmpMap[robot[i].pt.x][robot[i].pt.y] = 1;
                            if (tmpMap[np.x][np.y] == 1)
                                tmpMap[np.x][np.y] = 0;
                            addCmdIfInGood(np, i, outputCmds);
                            break;
                        }
                    }
                }
            }
        }
        else
        {
            auto tarBerth = robot[i].tarBerthIdx;
            if (tarBerth != -1) // 必不为-1
            {
                auto moveDir = berth[tarBerth].path[robot[i].pt.x][robot[i].pt.y];
                auto np = Util::calcNextPos(robot[i].pt, moveDir);
                if (tmpMap[np.x][np.y] != 0)
                {
                    outputCmds.push_back({CmdType::Move, i, moveDir});
                    tmpMap[robot[i].pt.x][robot[i].pt.y] = 1;
                    if (tmpMap[np.x][np.y] == 1)
                        tmpMap[np.x][np.y] = 0;
                    addCmdIfInBerth(np, i, outputCmds);
                }
                else
                {
                    std::vector<std::pair<int, std::pair<int, Point>>> idxDist;
                    for (int j = 0; j < 4; ++j)
                    {
                        if (j == moveDir)
                            continue;
                        np = Util::calcNextPos(robot[i].pt, j);
                        if (speedUpMap[np.x + Constancts::D1][np.y + Constancts::D1] &&
                            berth[tarBerth].dist[np.x][np.y] >= 0)
                        {
                            idxDist.push_back({j, {berth[tarBerth].dist[np.x][np.y], np}});
                        }
                    }
                    std::sort(idxDist.begin(),
                              idxDist.end(),
                              [](std::pair<int, std::pair<int, Point>> a, std::pair<int, std::pair<int, Point>> b) {
                                  return a.second.first < b.second.first;
                              });
                    for (int j = 0; j < idxDist.size(); ++j)
                    {
                        np = idxDist[j].second.second;
                        if (tmpMap[np.x][np.y] != 0)
                        {
                            outputCmds.push_back({CmdType::Move, i, idxDist[j].first});
                            if (tmpMap[robot[i].pt.x][robot[i].pt.y] == 0)
                                tmpMap[robot[i].pt.x][robot[i].pt.y] = 1;
                            if (tmpMap[np.x][np.y] == 1)
                                tmpMap[np.x][np.y] = 0;
                            addCmdIfInBerth(np, i, outputCmds);
                            break;
                        }
                    }
                }
            }
        }
    }
}

void findTarBerth(int boatId, double& maxVal, int& idx)
{
    std::vector<int> idxs;
    int maxv = -1, maxdist = -1;
    for (int j = 0; j < berthNum; ++j)
    {
        auto distToBerth = berth[j].berthingAreaDist[boat[boatId].pt.x][boat[boatId].pt.y][boat[boatId].dir];
        if (berth[j].goodsNum <= 0 || berth[j].isOwn || distToBerth < 0 ||
            (globalFrameID + distToBerth + berth[j].goodsNum / berth[j].loading_speed + berth[j].closedDPDis +
             Constancts::timeTolerance) > Constancts::allFrame)
            continue;

        idxs.push_back(j);
        maxdist = std::max(maxdist, distToBerth);
        maxv = std::max(maxv, berth[j].goodsVal);
    }

    for (int i = 0; i < idxs.size(); ++i)
    {
        // 2 1 0
        // 1
        // 3 0.5 0.5;
        auto curval = (0.9 * berth[idxs[i]].goodsVal / maxv);
        /*/ (0.1 * berth[idxs[i]].berthingAreaDist[boat[boatId].pt.x][boat[boatId].pt.y][boat[boatId].dir] / maxdist)*/

        if (curval > maxVal)
        {
            maxVal = curval;
            idx = idxs[i];
        }
    }
}

int switchBerthId(int boatId)
{
    std::vector<int> idxs;
    int maxv = -1, maxdist = -1;
    for (int j = 0; j < berthNum; ++j)
    {
        // 无货 || 被预约 || 不可达 || 去了来不及送到交货点
        auto distToBerth = berth[j].berthingAreaDist[boat[boatId].pt.x][boat[boatId].pt.y][boat[boatId].dir];
        if (berth[j].goodsNum <= 0 || berth[j].isOwn || distToBerth < 0 ||
            (globalFrameID + distToBerth + Constancts::timeTolerance + berth[j].goodsNum / berth[j].loading_speed +
                 berth[j].closedDPDis >
             Constancts::allFrame))
            continue;
        idxs.push_back(j);
        maxdist = std::max(maxdist, distToBerth);
        maxv = std::max(maxv, berth[j].goodsVal);
    }

    int idx2 = -1;
    double maxVal = -1;

    for (int i = 0; i < idxs.size(); ++i)
    {
        auto curval = (0.7 * berth[idxs[i]].goodsVal / maxv) /
            (0.3 * berth[idxs[i]].berthingAreaDist[boat[boatId].pt.x][boat[boatId].pt.y][boat[boatId].dir] / maxdist);

        if (curval > maxVal)
        {
            maxVal = curval;
            idx2 = idxs[i];
        }
    }

    return idx2;
}

void boatMove(std::vector<OutputCmd>& outputCmds)
{
    std::vector<int> moveBoat;
    for (int i = 0; i < boatNum; ++i)
    {
        if (boat[i].status == 1) // 恢复
            continue;
        if (boat[i].status == 2) // 装载
        {
            auto berthIdx = berthPosToIdx[boat[i].pt.x][boat[i].pt.y];
            if ((boat[i].goodsNum >= boat_capacity) ||
                (globalFrameID + berth[berthIdx].closedDPDis + Constancts::timeTolerance >=
                 Constancts::allFrame)) // 船满 找一个可达且最近的交货点
            {
                outputCmds.push_back({CmdType::Dept, i});
                boat[i].target = {false, berth[berthIdx].closedDPIdx};
                berth[berthIdx].isOwn = false;
            }
            else
            {
                if (berth[berthIdx].goodsNum <= 0) // 所在泊位无货 找另一个价值最高的泊位
                {
                    int idx2 = switchBerthId(i);
                    if (idx2 != -1) // 去另一个泊位
                    {
                        berth[idx2].isOwn = true;
                        berth[berthIdx].isOwn = false;
                        outputCmds.push_back({CmdType::Dept, i});
                        boat[i].target = {true, idx2};
                    }
                }
                else // 装货
                {
                    int goodsNum = std::min(berth[berthIdx].goodsNum, berth[berthIdx].loading_speed);
                    goodsNum = std::min(goodsNum, boat_capacity - boat[i].goodsNum);
                    berth[berthIdx].pullGoods(goodsNum);
                }
            }
        }
        else // 行驶状态
        {
            auto tarIdx = boat[i].target.second;
            if (tarIdx == -1)
            {
                int idx = 0;
                double maxVal = -1;
                for (int j = 0; j < berthNum; ++j)
                {
                    auto distToBerth = berth[j].berthingAreaDist[boat[i].pt.x][boat[i].pt.y][boat[i].dir];
                    if (berth[j].goodsNum <= 0 || berth[j].isOwn || distToBerth < 0)
                        continue;

                    if (berth[j].goodsVal > maxVal)
                    {
                        maxVal = berth[j].goodsVal;
                        idx = j;
                    }
                }
                berth[idx].isOwn = true;
                boat[i].target = {true, idx};
            }
            else
            {
                auto tarType = boat[i].target.first;
                if (!tarType) // 目的地类型为交货点
                {
                    if (boat[i].pt == deliveryPt[tarIdx].pt) // 位于交货点 清空货物 重新寻找目的泊位
                    {
                        boat[i].goodsNum = 0;
                        boat[i].target = {false, -1};
                        int idx = -1;
                        double maxVal = 0;
                        findTarBerth(i, maxVal, idx);
                        if (idx != -1)
                        {
                            berth[idx].isOwn = true;
                            boat[i].target = {true, idx};
                        }
                    }
                }
                else // 目的地类型为泊位
                {
                    if (berth[tarIdx].berthingAreaDist[boat[i].pt.x][boat[i].pt.y][boat[i].dir] == 0) // 位于靠泊区
                    {
                        outputCmds.push_back({CmdType::Berthing, i});
                        boat[i].target = {false, -1};
                    }
                    else
                    {
                        // 先判断有没有很近的交货点
                        bool isExistClosedDP = false;
                        auto curDis = berth[tarIdx].berthingAreaDist[boat[i].pt.x][boat[i].pt.y][boat[i].dir];
                        if (boat[i].goodsNum > 0)
                        {
                            int idx = -1;
                            int minDis = INT_MAX;
                            for (int j = 0; j < deliveryPt.size(); ++j)
                            {
                                if (deliveryPt[j].dist[boat[i].pt.x][boat[i].pt.y][boat[i].dir] >= 0 &&
                                    deliveryPt[j].dist[boat[i].pt.x][boat[i].pt.y][boat[i].dir] < minDis)
                                {
                                    minDis = deliveryPt[j].dist[boat[i].pt.x][boat[i].pt.y][boat[i].dir];
                                    idx = j;
                                }
                            }
                            if (idx != -1 && curDis > minDis * Constancts::D2)
                            {
                                isExistClosedDP = true;
                                boat[i].target = {false, idx};
                                berth[tarIdx].isOwn = false;
                            }
                        }
                        //// 不存在很近的交货点 更新目的泊位
                        // if (!isExistClosedDP)
                        //{
                        //     auto minNum = std::min(boat_capacity - boat[i].goodsNum, berth[tarIdx].goodsNum);
                        //     auto tmpGoods = berth[tarIdx].goods;
                        //     int maxVal = 0;
                        //     while (minNum)
                        //     {
                        //         maxVal += tmpGoods.front().val;
                        //         tmpGoods.pop();
                        //         --minNum;
                        //     }
                        //     int idx = -1;
                        //     for (int j = 0; j < berthNum; ++j)
                        //     {
                        //         auto distToBerth =
                        //         berth[j].berthingAreaDist[boat[i].pt.x][boat[i].pt.y][boat[i].dir]; if
                        //         (berth[j].isOwn || distToBerth < 0 || distToBerth > curDis)
                        //             continue;
                        //         // 计算能装多少价值的货物
                        //         minNum = std::min(boat_capacity - boat[i].goodsNum, berth[j].goodsNum);
                        //         tmpGoods = berth[j].goods;
                        //         int toVal = 0;
                        //         while (minNum)
                        //         {
                        //             toVal += tmpGoods.front().val;
                        //             tmpGoods.pop();
                        //             --minNum;
                        //         }
                        //         if (toVal > maxVal)
                        //         {
                        //             maxVal = toVal;
                        //             idx = j;
                        //         }
                        //     }
                        //     if (idx != -1)
                        //     {
                        //         berth[tarIdx].isOwn = false;
                        //         berth[idx].isOwn = true;
                        //         boat[i].target = {true, idx};
                        //     }
                        // }
                    }
                }
            }

            if (boat[i].target.second != -1)
            {
                moveBoat.push_back(i);
            }
        }
    }

    if (moveBoat.size() == 1)
    {
        auto tmpPath = boat[moveBoat[0]].target.first ? berth[boat[moveBoat[0]].target.second].berthingAreaPath
                                                      : deliveryPt[boat[moveBoat[0]].target.second].path;
        auto op = tmpPath[boat[moveBoat[0]].pt.x][boat[moveBoat[0]].pt.y][boat[moveBoat[0]].dir];
        op == 2 ? outputCmds.push_back({CmdType::Ship, moveBoat[0]})
                : outputCmds.push_back({CmdType::Rot, moveBoat[0], op});
    }
    else if (moveBoat.size() == 2)
    {
        // 遍历下一个机器人下一帧所有指令判断碰撞
        if (boat[0].pt.diatance(boat[1].pt) > Constancts::D3)
        {
            auto tmpPath = boat[0].target.first ? berth[boat[0].target.second].berthingAreaPath
                                                : deliveryPt[boat[0].target.second].path;
            auto op = tmpPath[boat[0].pt.x][boat[0].pt.y][boat[0].dir];
            op == 2 ? outputCmds.push_back({CmdType::Ship, 0}) : outputCmds.push_back({CmdType::Rot, 0, op});
            tmpPath = boat[1].target.first ? berth[boat[1].target.second].berthingAreaPath
                                           : deliveryPt[boat[1].target.second].path;
            op = tmpPath[boat[1].pt.x][boat[1].pt.y][boat[1].dir];
            op == 2 ? outputCmds.push_back({CmdType::Ship, 1}) : outputCmds.push_back({CmdType::Rot, 1, op});
        }
        else
        {
            std::unordered_set<Point, PointHash, PointEqual> boat1Pos;
            for (int i = 0; i < 2; ++i)
            {
                if (i == 0) // 第一艘船
                {
                    std::unordered_set<Point, PointHash, PointEqual> boat2Pos;
                    for (int k = 0; k < 6; ++k) // 存储第二艘船的位置
                    {
                        auto x = boat[1].pt.x + Constancts::boatBox[boat[1].dir][k][0];
                        auto y = boat[1].pt.y + Constancts::boatBox[boat[1].dir][k][1];
                        if (boatMap[x][y] == 1)
                        {
                            boat2Pos.insert(Point{x, y});
                        }
                    }

                    // auto tmpPathMap = boat[i].target.first ? berth[boat[i].target.second].berthingAreaPath
                    //                                        : deliveryPt[boat[i].target.second].path;
                    auto tmpDistMap = boat[i].target.first ? berth[boat[i].target.second].berthingAreaDist
                                                           : deliveryPt[boat[i].target.second].dist;
                    // std::pair<Point, int> curNP = {boat[i].pt, boat[i].dir};

                    std::stack<Opera> stack;
                    stack.push({-1, 0, boat[i].pt, boat[i].dir});

                    while (!stack.empty())
                    {
                        auto curNP = stack.top();

                        std::vector<Opera> pairs;
                        for (int k = 0; k < 3; ++k)
                        {
                            auto npd = Util::calcNextPos(curNP.nd, curNP.np, k);
                            if (tmpDistMap[npd.first.x][npd.first.y][npd.second] >= 0) // 排除-1(不能走的位置)
                            {
                                pairs.push_back(
                                    {k, tmpDistMap[npd.first.x][npd.first.y][npd.second], npd.first, npd.second});
                            }
                        }

                        std::sort(pairs.begin(), pairs.end(), [](Opera a, Opera b) { return a.dist < b.dist; });

                        int idx = -1;
                        for (int j = 0; j < pairs.size(); ++j)
                        {
                            auto np = pairs[j].np;
                            auto nd = pairs[j].nd;
                            bool isCollision = false;
                            for (int k = 0; k < 6; ++k)
                            {
                                auto x = np.x + Constancts::boatBox[nd][k][0];
                                auto y = np.y + Constancts::boatBox[nd][k][1];
                                if (boat2Pos.count(Point{x, y}))
                                {
                                    isCollision = true;
                                    break;
                                }
                            }
                            if (!isCollision)
                            {
                                idx = j;
                                break;
                            }
                        }
                        if (idx == -1)
                        {
                            stack.pop();
                        }
                        else
                        {
                            tmpDistMap[pairs[idx].np.x][pairs[idx].np.y][pairs[idx].nd] = -1;
                            stack.push({pairs[idx].op, 0, pairs[idx].np, pairs[idx].nd});
                            if (stack.size() >= 10)
                                break;
                        }
                    }

                    if (stack.size() <= 1)
                    {
                    }
                    else
                    {
                        auto tmpStack = stack;
                        while (!tmpStack.empty())
                        {
                            for (int k = 0; k < 6; ++k)
                            {
                                auto x = tmpStack.top().np.x + Constancts::boatBox[tmpStack.top().nd][k][0];
                                auto y = tmpStack.top().np.y + Constancts::boatBox[tmpStack.top().nd][k][1];
                                if (boatMap[x][y] == 1)
                                {
                                    boat1Pos.insert(Point{x, y});
                                }
                            }
                            tmpStack.pop();
                        }

                        int op = stack.top().op;
                        while (stack.size() != 2)
                        {
                            stack.pop();
                            op = stack.top().op;
                        }

                        op == 2 ? outputCmds.push_back({CmdType::Ship, i})
                                : outputCmds.push_back({CmdType::Rot, i, op});
                    }
                }
                else // 第二艘船
                {
                    auto tmpDistMap = boat[i].target.first ? berth[boat[i].target.second].berthingAreaDist
                                                           : deliveryPt[boat[i].target.second].dist;
                    std::vector<std::pair<int, int>> opDist;
                    for (int k = 0; k < 3; ++k)
                    {
                        auto npd = Util::calcNextPos(boat[i].dir, boat[i].pt, k);
                        if (tmpDistMap[npd.first.x][npd.first.y][npd.second] >= 0) // 排除-1(不能走的位置)
                        {
                            opDist.push_back({k, tmpDistMap[npd.first.x][npd.first.y][npd.second]});
                        }
                    }
                    std::sort(opDist.begin(), opDist.end(), [](std::pair<int, int> a, std::pair<int, int> b) {
                        return a.second < b.second;
                    });
                    for (int j = 0; j < opDist.size(); ++j)
                    {
                        auto npd = Util::calcNextPos(boat[i].dir, boat[i].pt, opDist[j].first);
                        bool isCollision = false;
                        for (int k = 0; k < 6; ++k)
                        {
                            auto x = npd.first.x + Constancts::boatBox[npd.second][k][0];
                            auto y = npd.first.y + Constancts::boatBox[npd.second][k][1];
                            if (boat1Pos.count(Point{x, y}))
                            {
                                isCollision = true;
                                break;
                            }
                        }
                        if (!isCollision)
                        {
                            opDist[j].first == 2 ? outputCmds.push_back({CmdType::Ship, i})
                                                 : outputCmds.push_back({CmdType::Rot, i, opDist[j].first});
                            break;
                        }
                    }
                }
            }
        }
    }
}

void deleteInvalidGoods()
{
    for (auto it = allGoods.begin(); it != allGoods.end();)
    {
        int restFrame = 1000 - globalFrameID + it->second.startFrame;
        if (restFrame <= 0)
        {
            goodsMap[it->second.pt.x][it->second.pt.y] = -1;
            it = allGoods.erase(it);
        }
        else
        {
            allGoods[it->first].restFrame = restFrame;
            ++it;
        }
    }
}

int main()
{
    Init();
    int count = 0;
    while (scanf("%d", &globalFrameID) != EOF)
    {
        //===============================deploy start===============================
        Input();
        // 删除过期的货物
        deleteInvalidGoods();
        std::vector<OutputCmd> outputCmds;
#if Scheme
        robotGetAndPull(outputCmds);
        robotMove(outputCmds);
#else
        robotGetAndPull1(outputCmds);
        robotMove1(outputCmds);
#endif // Scheme

        boatMove(outputCmds);
        buyRobot(outputCmds);
        buyBoat(outputCmds);
        printComands(outputCmds);
        //=============================== deploy end ===============================
        //===============================output start===============================
        ++count;
        if (globalFrameID == Constancts::allFrame)
        {
            std::cerr << "actual frame : " << count << std::endl;
            std::cerr << "robot num : " << robotNum << std::endl;
            for (int i = 0; i < boatNum; ++i)
            {
                std::cerr << "boat " << i << " least : " << boat[i].goodsNum << std::endl;
            }
            int leastNum = 0;
            int leastVal = 0;
            for (int i = 0; i < berthNum; ++i)
            {
                leastNum += berth[i].goodsNum;
                leastVal += berth[i].goodsVal;
            }
            std::cerr << "berth least num : " << leastNum << std::endl;
            std::cerr << "berth least val : " << leastVal << std::endl;
        }
        //=============================== output end ===============================
        puts("OK");
        fflush(stdout);
    }
    return 0;
}