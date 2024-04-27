#include "util.h"

void Util::preprocessGoods(std::vector<std::vector<int>> robotMap,
                           std::vector<std::vector<bool>> speedUpMap,
                           GoodInfo& gd)
{
    gd.path = std::vector<std::vector<int>>(Constancts::N, std::vector<int>(Constancts::N, -1));
    gd.dist = std::vector<std::vector<int>>(Constancts::N, std::vector<int>(Constancts::N, -1));
    std::queue<Point> queue;
    queue.push(gd.pt);
    gd.path[gd.pt.x][gd.pt.y] = -2; // 货物所在位置
    gd.dist[gd.pt.x][gd.pt.y] = 0;
    speedUpMap[gd.pt.x + Constancts::D1][gd.pt.y + Constancts::D1] = false;
    while (!queue.empty())
    {
        auto curPt = queue.front();
        queue.pop();
        int curDist = gd.dist[curPt.x][curPt.y];
        for (int j = 0; j < 4; ++j)
        {
            auto np = Util::calcNextPos(curPt, j);
            if (speedUpMap[np.x + Constancts::D1][np.y + Constancts::D1] && robotMap[np.x][np.y])
            {
                speedUpMap[np.x + Constancts::D1][np.y + Constancts::D1] = false;
                gd.path[np.x][np.y] = Constancts::reverseFourDirs[j];
                gd.dist[np.x][np.y] = curDist + 1;
                queue.push(np);
            }
        }
    }
}

void Util::initMapInfo(char map[Constancts::N][Constancts::N],
                       std::vector<std::vector<int>>& robotMap,
                       std::vector<std::vector<int>>& boatMap,
                       std::vector<PurchasePoint>& robotPurchasePt,
                       std::vector<PurchasePoint>& boatPurchasePt,
                       std::vector<DeliveryPoint>& deliveryPt)
{
    for (int i = 0; i < Constancts::N; ++i)
    {
        for (int j = 0; j < Constancts::N; ++j)
        {
            if (map[i][j] == 'T')
            {
                deliveryPt.push_back(DeliveryPoint{Point{i, j}});
            }
            if (map[i][j] == 'R')
            {
                robotPurchasePt.push_back(PurchasePoint{Point{i, j}});
            }
            if (map[i][j] == 'S')
            {
                boatPurchasePt.push_back(PurchasePoint{Point{i, j}});
            }
            if (map[i][j] == '.' || map[i][j] == 'C')
            {
                robotMap[i][j] = 1;
            }
            else if (map[i][j] == '>' || map[i][j] == 'R' || map[i][j] == 'B' || map[i][j] == 'c')
            {
                robotMap[i][j] = 2;
            }
            if (map[i][j] == '*' || map[i][j] == 'C')
            {
                boatMap[i][j] = 1;
            }
            else if (map[i][j] == '~' || map[i][j] == 'S' || map[i][j] == 'K' || map[i][j] == 'T' || map[i][j] == 'c' ||
                     map[i][j] == 'B')
            {
                boatMap[i][j] = 2;
            }
        }
    }
}

void Util::preprocessBerth(char map[Constancts::N][Constancts::N],
                           int berthNum,
                           std::vector<Berth>& berth,
                           std::vector<std::vector<int>> robotMap,
                           std::vector<std::vector<int>> boatMap)
{
    std::vector<std::vector<Point>> BS(berthNum);
    std::vector<std::vector<Point>> KS(berthNum);
    for (int i = 0; i < berthNum; ++i)
    {
        for (int m = -6; m < 6; ++m)
        {
            for (int n = -6; n < 6; ++n)
            {
                auto tmpPt = berth[i].pt + Point{m, n};
                if (Util::isValid(tmpPt) && map[tmpPt.x][tmpPt.y] == 'B')
                {
                    BS[i].push_back(tmpPt);
                    KS[i].push_back(tmpPt);
                }
                else if (Util::isValid(tmpPt) && map[tmpPt.x][tmpPt.y] == 'K')
                {
                    KS[i].push_back(tmpPt);
                }
            }
        }
    }
    // 陆地
    for (int i = 0; i < berthNum; ++i)
    {
        berth[i].path = std::vector<std::vector<int>>(Constancts::N, std::vector<int>(Constancts::N, -3));
        berth[i].dist = std::vector<std::vector<int>>(Constancts::N, std::vector<int>(Constancts::N, -1));
        std::queue<Point> queue;

        for (int j = 0; j < BS[i].size(); ++j)
        {
            queue.push(BS[i][j]);
            berth[i].path[BS[i][j].x][BS[i][j].y] = -2;
            berth[i].dist[BS[i][j].x][BS[i][j].y] = 0;
        }

        while (!queue.empty())
        {
            auto curPt = queue.front();
            queue.pop();
            int curDist = berth[i].dist[curPt.x][curPt.y];
            for (int j = 0; j < 4; ++j)
            {
                auto np = Util::calcNextPos(curPt, j);
                if (Util::isValid(np))
                {
                    if (berth[i].path[np.x][np.y] == -3)
                    {
                        if (robotMap[np.x][np.y])
                        {
                            berth[i].path[np.x][np.y] = Constancts::reverseFourDirs[j];
                            berth[i].dist[np.x][np.y] = curDist + 1;
                            queue.push(np);
                        }
                        else
                        {
                            berth[i].path[np.x][np.y] = -1;
                            berth[i].dist[np.x][np.y] = -1;
                        }
                    }
                }
            }
        }
    }
    // 海洋path
    for (int i = 0; i < berthNum; ++i)
    {
        berth[i].berthingAreaPath = std::vector<std::vector<std::vector<int>>>(
            Constancts::N, std::vector<std::vector<int>>(Constancts::N, std::vector<int>(4, -1)));
        berth[i].berthingAreaDist = std::vector<std::vector<std::vector<int>>>(
            Constancts::N, std::vector<std::vector<int>>(Constancts::N, std::vector<int>(4, -1)));
        std::queue<std::pair<Point, int>> queue;
        for (int j = 0; j < KS[i].size(); ++j)
        {
            for (int k = 0; k < 4; ++k)
            {
                if (isValidOfBoat({KS[i][j], k}, boatMap))
                {
                    queue.push({KS[i][j], k});
                    berth[i].berthingAreaDist[KS[i][j].x][KS[i][j].y][k] = 0;
                }
            }
        }
        while (!queue.empty())
        {
            auto cp = queue.front().first;
            auto cd = queue.front().second;
            auto curDist = berth[i].berthingAreaDist[cp.x][cp.y][cd];

            queue.pop();
            for (int k = 0; k < 3; ++k)
            {
                auto lastPD = reverseCalcNextPos(cd, cp, k);

                bool isValid = true;
                int dist = 1;
                for (int d = 0; d < 6; ++d)
                {
                    auto x = lastPD.first.x + Constancts::boatBox[lastPD.second][d][0];
                    auto y = lastPD.first.y + Constancts::boatBox[lastPD.second][d][1];
                    if (!Util::isValid(Point{x, y}))
                        isValid = false;
                    else
                    {
                        if (boatMap[x][y] == 0)
                            isValid = false;
                        else if (boatMap[x][y] == 2)
                            dist = 2;
                    }
                }

                if (isValid &&
                    (berth[i].berthingAreaDist[lastPD.first.x][lastPD.first.y][lastPD.second] == -1 ||
                     berth[i].berthingAreaDist[lastPD.first.x][lastPD.first.y][lastPD.second] > curDist + dist))
                {
                    berth[i].berthingAreaPath[lastPD.first.x][lastPD.first.y][lastPD.second] = k;
                    berth[i].berthingAreaDist[lastPD.first.x][lastPD.first.y][lastPD.second] = curDist + dist;
                    queue.push(lastPD);
                }
            }
        }
    }
}

void Util::preprocessRobotPurchasePoint(std::vector<std::vector<int>> robotMap,
                                        std::vector<PurchasePoint>& robotPurchasePt)
{
    for (int i = 0; i < robotPurchasePt.size(); ++i)
    {
        robotPurchasePt[i].dist = std::vector<std::vector<int>>(Constancts::N, std::vector<int>(Constancts::N, -2));
        std::queue<Point> queue;
        queue.push(robotPurchasePt[i].pt);
        robotPurchasePt[i].dist[robotPurchasePt[i].pt.x][robotPurchasePt[i].pt.y] = 0;
        while (!queue.empty())
        {
            auto curPt = queue.front();
            queue.pop();
            int curDist = robotPurchasePt[i].dist[curPt.x][curPt.y];
            for (int j = 0; j < 4; ++j)
            {
                auto np = Util::calcNextPos(curPt, j);
                if (Util::isValid(np) && robotPurchasePt[i].dist[np.x][np.y] == -2)
                {
                    if (robotMap[np.x][np.y])
                    {
                        robotPurchasePt[i].dist[np.x][np.y] = curDist + 1;
                        queue.push(np);
                    }
                    else
                    {
                        robotPurchasePt[i].dist[np.x][np.y] = -1;
                    }
                }
            }
        }
    }
}

void Util::preprocessBoatPurchasePoint(std::vector<std::vector<int>> boatMap,
                                       std::vector<PurchasePoint>& boatPurchasePt)
{
    for (int i = 0; i < boatPurchasePt.size(); ++i)
    {
        boatPurchasePt[i].dist = std::vector<std::vector<int>>(Constancts::N, std::vector<int>(Constancts::N, -2));
        std::queue<Point> queue;
        queue.push(boatPurchasePt[i].pt);
        boatPurchasePt[i].dist[boatPurchasePt[i].pt.x][boatPurchasePt[i].pt.y] = 0;

        while (!queue.empty())
        {
            auto curPt = queue.front();
            queue.pop();
            int curDist = boatPurchasePt[i].dist[curPt.x][curPt.y];
            for (int j = 0; j < 4; ++j)
            {
                auto np = Util::calcNextPos(curPt, j);
                if (Util::isValid(np) && boatPurchasePt[i].dist[np.x][np.y] == -2)
                {
                    if (boatMap[np.x][np.y])
                    {
                        boatPurchasePt[i].dist[np.x][np.y] = curDist + 1;
                        queue.push(np);
                    }
                    else
                    {
                        boatPurchasePt[i].dist[np.x][np.y] = -1;
                    }
                }
            }
        }
    }
}

std::pair<Point, int> Util::reverseCalcNextPos(int boatDir, Point boatPt, int op)
{
    if (op == 1) // anticlockwise
    {
        switch (boatDir)
        {
        case 0:
            return {Point{boatPt.x - 1, boatPt.y + 1}, 3};
        case 1:
            return {Point{boatPt.x + 1, boatPt.y - 1}, 2};
        case 2:
            return {Point{boatPt.x - 1, boatPt.y - 1}, 0};
        case 3:
            return {Point{boatPt.x + 1, boatPt.y + 1}, 1};
        default:
            break;
        }
    }
    else if (op == 2) // forward
    {
        switch (boatDir)
        {
        case 0:
            return {Point{boatPt.x, boatPt.y - 1}, boatDir};
        case 1:
            return {Point{boatPt.x, boatPt.y + 1}, boatDir};
        case 2:
            return {Point{boatPt.x + 1, boatPt.y}, boatDir};
        case 3:
            return {Point{boatPt.x - 1, boatPt.y}, boatDir};
        default:
            break;
        }
    }
    else // clockwise
    {
        switch (boatDir)
        {
        case 0:
            return {Point{boatPt.x + 2, boatPt.y}, 2};
        case 1:
            return {Point{boatPt.x - 2, boatPt.y}, 3};
        case 2:
            return {Point{boatPt.x, boatPt.y + 2}, 1};
        case 3:
            return {Point{boatPt.x, boatPt.y - 2}, 0};
        default:
            break;
        }
    }
}

std::pair<Point, int> Util::calcNextPos(int boatDir, Point boatPt, int op)
{
    if (op == 1) // anticlockwise
    {
        switch (boatDir)
        {
        case 0:
            return {Point{boatPt.x + 1, boatPt.y + 1}, 2};
        case 1:
            return {Point{boatPt.x - 1, boatPt.y - 1}, 3};
        case 2:
            return {Point{boatPt.x - 1, boatPt.y + 1}, 1};
        case 3:
            return {Point{boatPt.x + 1, boatPt.y - 1}, 0};
        default:
            break;
        }
    }
    else if (op == 2) // forward
    {
        switch (boatDir)
        {
        case 0:
            return {Point{boatPt.x, boatPt.y + 1}, boatDir};
        case 1:
            return {Point{boatPt.x, boatPt.y - 1}, boatDir};
        case 2:
            return {Point{boatPt.x - 1, boatPt.y}, boatDir};
        case 3:
            return {Point{boatPt.x + 1, boatPt.y}, boatDir};
        default:
            break;
        }
    }
    else // clockwise
    {
        switch (boatDir)
        {
        case 0:
            return {Point{boatPt.x, boatPt.y + 2}, 3};
        case 1:
            return {Point{boatPt.x, boatPt.y - 2}, 2};
        case 2:
            return {Point{boatPt.x - 2, boatPt.y}, 0};
        case 3:
            return {Point{boatPt.x + 2, boatPt.y}, 1};
        default:
            break;
        }
    }
}

bool Util::isValidOfBoat(std::pair<Point, int> pd, std::vector<std::vector<int>> boatMap)
{
    for (int i = 0; i < 6; ++i)
    {
        auto x = pd.first.x + Constancts::boatBox[pd.second][i][0];
        auto y = pd.first.y + Constancts::boatBox[pd.second][i][1];
        if (!isValid(Point{x, y}) || !boatMap[x][y])
        {
            return false;
        }
    }

    return true;
}

void Util::preprocessDeliveryPoint(std::vector<std::vector<int>> boatMap, std::vector<DeliveryPoint>& deliveryPt)
{
    for (int i = 0; i < deliveryPt.size(); ++i)
    {
        deliveryPt[i].path = std::vector<std::vector<std::vector<int>>>(
            Constancts::N, std::vector<std::vector<int>>(Constancts::N, std::vector<int>(4, -1)));
        deliveryPt[i].dist = std::vector<std::vector<std::vector<int>>>(
            Constancts::N, std::vector<std::vector<int>>(Constancts::N, std::vector<int>(4, -1)));
        std::queue<std::pair<Point, int>> queue;
        for (int j = 0; j < 4; ++j)
        {
            if (isValidOfBoat({deliveryPt[i].pt, j}, boatMap))
            {
                queue.push({deliveryPt[i].pt, j});
                deliveryPt[i].dist[deliveryPt[i].pt.x][deliveryPt[i].pt.y][j] = 0;
            }
        }
        while (!queue.empty())
        {
            auto cp = queue.front().first;
            auto cd = queue.front().second;
            auto curDist = deliveryPt[i].dist[cp.x][cp.y][cd];
            queue.pop();
            for (int k = 0; k < 3; ++k)
            {
                auto lastPD = reverseCalcNextPos(cd, cp, k);

                bool isValid = true;
                int dist = 1;
                for (int d = 0; d < 6; ++d)
                {
                    auto x = lastPD.first.x + Constancts::boatBox[lastPD.second][d][0];
                    auto y = lastPD.first.y + Constancts::boatBox[lastPD.second][d][1];
                    if (!Util::isValid(Point{x, y}))
                        isValid = false;
                    else
                    {
                        if (boatMap[x][y] == 0)
                            isValid = false;
                        else if (boatMap[x][y] == 2)
                            dist = 2;
                    }
                }

                if (isValid &&
                    (deliveryPt[i].dist[lastPD.first.x][lastPD.first.y][lastPD.second] == -1 ||
                     deliveryPt[i].dist[lastPD.first.x][lastPD.first.y][lastPD.second] > curDist + dist))
                {
                    deliveryPt[i].path[lastPD.first.x][lastPD.first.y][lastPD.second] = k;
                    deliveryPt[i].dist[lastPD.first.x][lastPD.first.y][lastPD.second] = curDist + dist;
                    queue.push(lastPD);
                }
            }
        }
    }
}

void Util::preprocess(char map[Constancts::N][Constancts::N],
                      int berthNum,
                      std::vector<Berth>& berth,
                      std::vector<std::vector<int>>& robotMap,
                      std::vector<std::vector<int>>& boatMap,
                      std::vector<PurchasePoint>& robotPurchasePt,
                      std::vector<PurchasePoint>& boatPurchasePt,
                      std::vector<DeliveryPoint>& deliveryPt)
{
    //
    Util::initMapInfo(map, robotMap, boatMap, robotPurchasePt, boatPurchasePt, deliveryPt);
    //
    Util::preprocessBerth(map, berthNum, berth, robotMap, boatMap);
    //
    Util::preprocessRobotPurchasePoint(robotMap, robotPurchasePt);
    //
    Util::preprocessBoatPurchasePoint(boatMap, boatPurchasePt);
    //
    Util::preprocessDeliveryPoint(boatMap, deliveryPt);
    // 找到每个泊位最近的交货点
    for (int i = 0; i < berthNum; ++i)
    {
        berth[i].closedDPIdx = -1;
        berth[i].closedDPDis = INT_MAX;

        for (int j = 0; j < deliveryPt.size(); ++j)
        {
            int maxDist = -1;
            for (int k = 0; k < 4; ++k)
            {
                maxDist = std::max(berth[i].berthingAreaDist[deliveryPt[j].pt.x][deliveryPt[j].pt.y][k], maxDist);
            }

            if (maxDist > 0 && maxDist < berth[i].closedDPDis)
            {
                berth[i].closedDPDis = maxDist;
                berth[i].closedDPIdx = j;
            }
        }
    }
}

Point Util::calcNextPos(Point pos, int moveDir)
{
    Point moveDis;
    switch (moveDir)
    {
    case 0:
    {
        moveDis = Point{0, 1};
        break;
    }
    case 1:
    {
        moveDis = Point{0, -1};
        break;
    }
    case 2:
    {
        moveDis = Point{-1, 0};
        break;
    }
    case 3:
    {
        moveDis = Point{1, 0};
        break;
    }
    default:
        break;
    }
    return pos + moveDis;
}

int Util::posToMoveDir(Point curPos, Point nextPos)
{
    auto dis = nextPos - curPos;

    if (dis == Point{1, 0})
    {
        return 3;
    }
    else if (dis == Point{-1, 0})
    {
        return 2;
    }
    else if (dis == Point{0, -1})
    {
        return 1;
    }
    else if (dis == Point{0, 1})
    {
        return 0;
    }

    return -1;
}

bool Util::isValid(Point pt)
{
    return (pt.x >= 0) && (pt.x < Constancts::N) && (pt.y >= 0) && (pt.y < Constancts::N);
}

bool Util::isValid(Point pt, char map[Constancts::N][Constancts::N], Box box)
{
    return (pt.x >= box.minB.x) && (pt.x < box.maxB.x) && (pt.y >= box.minB.y) && (pt.y < box.maxB.y) &&
        map[pt.x][pt.y] != '*' && map[pt.x][pt.y] != '#';
}

std::stack<int> Util::findPathByBFS(Point start,
                                    Point end,
                                    char map[Constancts::N][Constancts::N],
                                    int range,
                                    std::unordered_set<Point, PointHash, PointEqual> otherRobotPos)
{
    auto box =
        Box{{std::min(start.x, end.x), std::min(start.y, end.y)}, {std::max(start.x, end.x), std::max(start.y, end.y)}};
    box.minB.x = std::max(0, box.minB.x - range);
    box.minB.y = std::max(0, box.minB.y - range);
    box.maxB.x = std::min(199, box.maxB.x + range);
    box.maxB.y = std::min(199, box.maxB.y + range);

    std::vector<std::vector<bool>> visited(Constancts::N, std::vector<bool>(Constancts::N, false));
    std::vector<std::vector<Point>> parents(Constancts::N, std::vector<Point>(Constancts::N, Point{-1, -1}));
    std::queue<Point> queue;
    queue.push(start);
    visited[start.x][start.y] = true;

    while (!queue.empty())
    {
        auto current = queue.front();
        queue.pop();

        if (current == end)
        {
            std::stack<int> ret;
            auto np = parents[current.x][current.y];
            while (np.x != -1 && np.y != -1)
            {
                ret.push(Util::posToMoveDir(current, np));
                current = np;
                np = parents[current.x][current.y];
            }

            return ret;
        }

        for (int j = 0; j < 4; ++j)
        {
            auto np = Util::calcNextPos(current, j);
            if (Util::isValid(np, map, box) && !visited[np.x][np.y] && !otherRobotPos.count(np))
            {
                queue.push(np);
                visited[np.x][np.y] = true;
                parents[np.x][np.y] = current;
            }
        }
    }

    return std::stack<int>();
}
