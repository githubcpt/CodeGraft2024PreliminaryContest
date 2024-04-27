#pragma once

struct Constancts
{
    static constexpr int N = 200; // 地图维度
    static constexpr int D1 = 5;  // 数组防止越界维度
    static constexpr char* cmdToChar[9] = {"move", "get", "pull", "rot", "ship", "berth", "dept", "lboat", "lbot"};
    static constexpr int boatPrice = 8000;
    static constexpr int robotPrice = 2000;
    static constexpr int allFrame = 15000;
    static constexpr int fourDirs[4][4] = {{2, 0, 3, 1}, {0, 3, 1, 2}, {3, 1, 2, 0}, {1, 2, 0, 3}};
    static constexpr int reverseFourDirs[4] = {1, 0, 3, 2};
    static constexpr int boatBox[4][6][2] = {{{0, 0}, {0, 1}, {0, 2}, {1, 0}, {1, 1}, {1, 2}},
                                             {{0, 0}, {0, -1}, {0, -2}, {-1, 0}, {-1, -1}, {-1, -2}},
                                             {{0, 0}, {0, 1}, {-1, 0}, {-1, 1}, {-2, 0}, {-2, 1}},
                                             {{0, 0}, {0, -1}, {1, 0}, {1, -1}, {2, 0}, {2, -1}}};
    static constexpr int timeTolerance = 10; // 交货时间容差
    static constexpr int D2 = 5;             // 轮船距离新地点容差
    static constexpr int D3 = 12;            // 轮船碰撞容差
};

// 0 表示右移一格
// 1 表示左移一格
// 2 表示上移一格
// 3 表示下移一格

//  .  : 空地
//  >  : 陆地主干道，在主干道上移动的机器人不会发生碰撞。
//  #  : 障碍
//  R  : 机器人购买地块，同时该地块也是主干道
//  B  : 泊位，可以视为一种特殊的海陆立体交通地块，并同时被视为主干道和主航道

//  C  : 海陆立体交通地块,机器人和船舶皆可在该类地块中行驶，机器人和船舶之间不会发生碰撞
//  c  : 海陆立体交通地块，同时为主干道和主航道

//  *  : 海洋
//  ~  : 海洋主航道，在主航道上行驶的船不会发生碰撞，但在该区域行驶的船的速度会变慢。
//  S  : 船舶购买地块，同时该地块也是主航道
//  K  : 靠泊区，可被视为一种特殊的海洋主航道地块
//  T  : 交货点,可以视为一种特殊的靠泊区，运输船只到达该区域后可立即获取相应运输价值。

// 靠泊区可被视为一种特殊的海洋主航道地块。每个泊位与靠泊区形成的四连通
// 的连通块中不会包括其他泊位。当船只核心点位于靠泊区或泊位区域内时，
// 该船可以执行靠泊指令，此时该船只将自动停靠到该靠泊区内的对应泊位上。


/*
void robotMove(std::vector<OutputCmd>& outputCmds)
{
    std::vector<int> robotMoveDir(robotNum, -1);
    std::vector<bool> isPullRobot(robotNum, false);
    for (int i = 0; i < robotNum; i++)
    {
        if (robot[i].have_goods)
        {
            isPullRobot[i] = true;
            auto tarBerth = robot[i].tarBerthIdx;
            if (tarBerth != -1)
            {
                robotMoveDir[i] = berth[tarBerth].path[robot[i].pt.x][robot[i].pt.y];
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
                robotMoveDir[i] = allGoods[tarGoods].path[robot[i].pt.x][robot[i].pt.y];
            }
        }
    }

    auto tmpMap = robotMap;
    for (int i = 0; i < robotNum; i++)
    {
        if (robotMap[robot[i].pt.x][robot[i].pt.y] == 1)
            tmpMap[robot[i].pt.x][robot[i].pt.y] = 0;
    }

    int idx = 0;
    for (int i = 0; i < robotNum; i++)
    {
        if (robotMoveDir[i] == -1)
            continue;
        auto moveDir = robotMoveDir[i];
        auto np = Util::calcNextPos(robot[i].pt, moveDir);
        if (tmpMap[np.x][np.y] != 0)
        {
            outputCmds.push_back({CmdType::Move, i, moveDir});
            tmpMap[robot[i].pt.x][robot[i].pt.y] = 1;
            if (tmpMap[np.x][np.y] == 1)
                tmpMap[np.x][np.y] = 0;
            isPullRobot[i] ? addCmdIfInBerth(np, i, outputCmds) : addCmdIfInGood(np, i, outputCmds);
        }
        else
        {
            for (int j = 0; j < 4; j++)
            {
                if (Constancts::fourDirs[idx % 4][j] == moveDir)
                    continue;
                auto dir = Constancts::fourDirs[idx % 4][j];
                np = Util::calcNextPos(robot[i].pt, dir);
                if (tmpMap[np.x][np.y] != 0)
                {
                    ++idx;
                    outputCmds.push_back({CmdType::Move, i, dir});
                    if (tmpMap[robot[i].pt.x][robot[i].pt.y] == 0)
                        tmpMap[robot[i].pt.x][robot[i].pt.y] = 1;
                    if (tmpMap[np.x][np.y] == 1)
                        tmpMap[np.x][np.y] = 0;
                    isPullRobot[i] ? addCmdIfInBerth(np, i, outputCmds) : addCmdIfInGood(np, i, outputCmds);
                    break;
                }
            }
        }
    }
}
*/

// if (i == 0) // 第一艘船
//{
//     std::unordered_set<Point, PointHash, PointEqual> boat2Pos;
//     for (int k = 0; k < 6; ++k) // 存储第二艘船的位置
//     {
//         auto x = boat[1].pt.x + Constancts::boatBox[boat[1].dir][k][0];
//         auto y = boat[1].pt.y + Constancts::boatBox[boat[1].dir][k][1];
//         if (boatMap[x][y] == 1)
//         {
//             boat2Pos.insert(Point{ x, y });
//         }
//     }
//
//     // auto tmpPathMap = boat[i].target.first ? berth[boat[i].target.second].berthingAreaPath
//     //                                        : deliveryPt[boat[i].target.second].path;
//     auto tmpDistMap = boat[i].target.first ? berth[boat[i].target.second].berthingAreaDist
//         : deliveryPt[boat[i].target.second].dist;
//     // std::pair<Point, int> curNP = {boat[i].pt, boat[i].dir};
//
//     std::stack<Opera> stack;
//     stack.push({ -1, 0, boat[i].pt, boat[i].dir });
//
//     while (!stack.empty())
//     {
//         auto curNP = stack.top();
//
//         std::vector<Opera> pairs;
//         for (int k = 0; k < 3; ++k)
//         {
//             auto npd = Util::calcNextPos(curNP.nd, curNP.np, k);
//             if (tmpDistMap[npd.first.x][npd.first.y][npd.second] >= 0) // 排除-1(不能走的位置)
//             {
//                 pairs.push_back(
//                     { k, tmpDistMap[npd.first.x][npd.first.y][npd.second], npd.first, npd.second });
//             }
//         }
//
//         std::sort(pairs.begin(), pairs.end(), [](Opera a, Opera b) { return a.dist < b.dist; });
//
//         int idx = -1;
//         for (int j = 0; j < pairs.size(); ++j)
//         {
//             auto np = pairs[j].np;
//             auto nd = pairs[j].nd;
//             bool isCollision = false;
//             for (int k = 0; k < 6; ++k)
//             {
//                 auto x = np.x + Constancts::boatBox[nd][k][0];
//                 auto y = np.y + Constancts::boatBox[nd][k][1];
//                 if (boat2Pos.count(Point{ x, y }))
//                 {
//                     isCollision = true;
//                     break;
//                 }
//             }
//             if (!isCollision)
//             {
//                 idx = j;
//                 break;
//             }
//         }
//         if (idx == -1)
//         {
//             stack.pop();
//         }
//         else
//         {
//             tmpDistMap[pairs[idx].np.x][pairs[idx].np.y][pairs[idx].nd] = -1;
//             stack.push({ pairs[idx].op, 0, pairs[idx].np, pairs[idx].nd });
//             if (stack.size() >= 10)
//                 break;
//         }
//     }
//
//     if (stack.size() <= 1)
//     {
//     }
//     else
//     {
//         auto tmpStack = stack;
//         while (!tmpStack.empty())
//         {
//             for (int k = 0; k < 6; ++k)
//             {
//                 auto x = tmpStack.top().np.x + Constancts::boatBox[tmpStack.top().nd][k][0];
//                 auto y = tmpStack.top().np.y + Constancts::boatBox[tmpStack.top().nd][k][1];
//                 if (boatMap[x][y] == 1)
//                 {
//                     boat1Pos.insert(Point{ x, y });
//                 }
//             }
//             tmpStack.pop();
//         }
//
//         int op = stack.top().op;
//         while (stack.size() != 2)
//         {
//             stack.pop();
//             op = stack.top().op;
//         }
//
//         op == 2 ? outputCmds.push_back({ CmdType::Ship, i })
//             : outputCmds.push_back({ CmdType::Rot, i, op });
//     }
// }
// else // 第二艘船
//{
//     auto tmpDistMap = boat[i].target.first ? berth[boat[i].target.second].berthingAreaDist
//         : deliveryPt[boat[i].target.second].dist;
//     std::vector<std::pair<int, int>> opDist;
//     for (int k = 0; k < 3; ++k)
//     {
//         auto npd = Util::calcNextPos(boat[i].dir, boat[i].pt, k);
//         if (tmpDistMap[npd.first.x][npd.first.y][npd.second] >= 0) // 排除-1(不能走的位置)
//         {
//             opDist.push_back({ k, tmpDistMap[npd.first.x][npd.first.y][npd.second] });
//         }
//     }
//     std::sort(opDist.begin(), opDist.end(), [](std::pair<int, int> a, std::pair<int, int> b) {
//         return a.second < b.second;
//         });
//     for (int j = 0; j < opDist.size(); ++j)
//     {
//         auto npd = Util::calcNextPos(boat[i].dir, boat[i].pt, opDist[j].first);
//         bool isCollision = false;
//         for (int k = 0; k < 6; ++k)
//         {
//             auto x = npd.first.x + Constancts::boatBox[npd.second][k][0];
//             auto y = npd.first.y + Constancts::boatBox[npd.second][k][1];
//             if (boat1Pos.count(Point{ x, y }))
//             {
//                 isCollision = true;
//                 break;
//             }
//         }
//         if (!isCollision)
//         {
//             opDist[j].first == 2 ? outputCmds.push_back({ CmdType::Ship, i })
//                 : outputCmds.push_back({ CmdType::Rot, i, opDist[j].first });
//             break;
//         }
//     }
// }

//// 不存在很近的交货点 寻找是否存在更近且有货的泊位
// if (!isExistClosedDP)
//{
//    int idx = -1;
//    int minDis = INT_MAX;
//    for (int j = 0; j < berthNum; ++j)
//    {
//        // 被预约 || 货物数量小于5 || 不可达
//        if (berth[j].isOwn || berth[j].goodsVal <= berth[tarIdx].goodsVal ||
//            berth[j].berthingAreaDist[boat[i].pt.x][boat[i].pt.y][boat[i].dir] < 0)
//            continue;
//        if (berth[j].berthingAreaDist[boat[i].pt.x][boat[i].pt.y][boat[i].dir] < minDis)
//        {
//            minDis = berth[j].berthingAreaDist[boat[i].pt.x][boat[i].pt.y][boat[i].dir];
//            idx = j;
//        }
//    }
//    if (idx != -1 && curDis > minDis * Constancts::D2)
//    {
//        berth[tarIdx].isOwn = false;
//        berth[idx].isOwn = true;
//        if (minDis == 0) // 已经在更近且有货的泊位范围内
//        {
//            outputCmds.push_back({CmdType::Berthing, i});
//            boat[i].target = {false, -1};
//        }
//        else
//        {
//            boat[i].target = {true, idx};
//        }
//    }
//}

/*
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
                // auto moveDir = berth[tarBerth].path[robot[i].pt.x][robot[i].pt.y];
                // auto np = Util::calcNextPos(robot[i].pt, moveDir);
                // if (tmpMap[np.x][np.y] != 0)
                //{
                //     outputCmds.push_back({CmdType::Move, i, moveDir});
                //     tmpMap[robot[i].pt.x][robot[i].pt.y] = 1;
                //     if (tmpMap[np.x][np.y] == 1)
                //         tmpMap[np.x][np.y] = 0;
                //     addCmdIfInBerth(np, i, outputCmds);
                // }
                // else
                {
                    std::vector<std::pair<int, std::pair<int, Point>>> idxDist;
                    for (int j = 0; j < 4; ++j)
                    {
                        // if (j == moveDir)
                        //     continue;
                        auto np = Util::calcNextPos(robot[i].pt, j);
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
                        auto np = idxDist[j].second.second;
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
                updateBetterGoods(i, outputCmds);
            }
            else // 初始状态 || 送完货物 || 货物消失
            {
                getBestGoods(i);
            }

            auto tarGoods = robot[i].tarGoodIdx;
            if (tarGoods != -1)
            {
                // auto moveDir = allGoods[tarGoods].path[robot[i].pt.x][robot[i].pt.y];
                // if (moveDir == -2) // 更新货物时 最好的货物在脚下
                //{
                //     addCmdIfInGood(robot[i].pt, i, outputCmds);
                //     continue;
                // }
                // auto np = Util::calcNextPos(robot[i].pt, moveDir);
                // if (tmpMap[np.x][np.y] != 0)
                //{
                //     outputCmds.push_back({CmdType::Move, i, moveDir});
                //     tmpMap[robot[i].pt.x][robot[i].pt.y] = 1;
                //     if (tmpMap[np.x][np.y] == 1)
                //         tmpMap[np.x][np.y] = 0;
                //     addCmdIfInGood(np, i, outputCmds);
                // }
                // else
                {
                    std::vector<std::pair<int, std::pair<int, Point>>> idxDist;
                    for (int j = 0; j < 4; ++j)
                    {
                        // if (j == moveDir)
                        //     continue;
                        auto np = Util::calcNextPos(robot[i].pt, j);
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
                        auto np = idxDist[j].second.second;
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
*/

/*
void getIdxDist(int ridx,
                std::vector<std::vector<int>> dist,
                std::vector<std::pair<int, std::pair<int, Point>>>& idxDist)
{
    for (int j = 0; j < 4; ++j)
    {
        auto np = Util::calcNextPos(robot[ridx].pt, j);
        if (speedUpMap[np.x + Constancts::D1][np.y + Constancts::D1] && dist[np.x][np.y] >= 0)
        {
            idxDist.push_back({j, {dist[np.x][np.y], np}});
        }
    }
    std::sort(idxDist.begin(),
              idxDist.end(),
              [](std::pair<int, std::pair<int, Point>> a, std::pair<int, std::pair<int, Point>> b) {
                  return a.second.first < b.second.first;
              });
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
                std::vector<std::pair<int, std::pair<int, Point>>> idxDist;
                getIdxDist(i, berth[tarBerth].dist, idxDist);
                for (int j = 0; j < idxDist.size(); ++j)
                {
                    auto np = idxDist[j].second.second;
                    if (tmpMap[np.x][np.y] != 0)
                    {
                        outputCmds.push_back({CmdType::Move, i, idxDist[j].first});
                        if (tmpMap[robot[i].pt.x][robot[i].pt.y] == 0)
                            tmpMap[robot[i].pt.x][robot[i].pt.y] = 1;
                        if (tmpMap[np.x][np.y] == 1)
                            tmpMap[np.x][np.y] = 0;
                        if (idxDist[j].second.first == 0)
                        {
                            outputCmds.push_back({CmdType::Pull, i});
                            berth[tarBerth].getGoods(robot[i].pullGoods());
                        }
                        break;
                    }
                }
            }
        }
        else
        {
            if (robot[i].tarGoodIdx != -1 && allGoods.count(robot[i].tarGoodIdx))
            {
                updateBetterGoods(i, outputCmds);
            }
            else // 初始状态 || 送完货物 || 货物消失
            {
                getBestGoods(i);
            }

            auto tarGoods = robot[i].tarGoodIdx;
            if (tarGoods != -1)
            {
                if (allGoods[tarGoods].dist[robot[i].pt.x][robot[i].pt.y] == 0)
                {
                    outputCmds.push_back({CmdType::Get, i});
                    robot[i].getGoods(allGoods[tarGoods]);
                    allGoods.erase(tarGoods);
                    goodsMap[robot[i].pt.x][robot[i].pt.y] = -1;
                }
                else
                {
                    std::vector<std::pair<int, std::pair<int, Point>>> idxDist;
                    getIdxDist(i, allGoods[tarGoods].dist, idxDist);
                    for (int j = 0; j < idxDist.size(); ++j)
                    {
                        auto np = idxDist[j].second.second;
                        if (tmpMap[np.x][np.y] != 0)
                        {
                            outputCmds.push_back({CmdType::Move, i, idxDist[j].first});
                            if (tmpMap[robot[i].pt.x][robot[i].pt.y] == 0)
                                tmpMap[robot[i].pt.x][robot[i].pt.y] = 1;
                            if (tmpMap[np.x][np.y] == 1)
                                tmpMap[np.x][np.y] = 0;
                            if (idxDist[j].second.first == 0)
                            {
                                outputCmds.push_back({CmdType::Get, i});
                                robot[i].getGoods(allGoods[tarGoods]);
                                allGoods.erase(tarGoods);
                                goodsMap[np.x][np.y] = -1;
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
}
*/