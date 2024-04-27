#pragma once
#include "constants.h"
#include "object.h"

class Util
{
public:
    Util();
    ~Util();

    static Point calcNextPos(Point pos, int moveDir);

    static int posToMoveDir(Point curPos, Point nextPos);

    static bool isValid(Point pt);
    static bool isValid(Point pt, char map[Constancts::N][Constancts::N], Box box = {{0, 0}, {200, 200}});

    static std::stack<int> findPathByBFS(Point start,
                                         Point end,
                                         char map[Constancts::N][Constancts::N],
                                         int range,
                                         std::unordered_set<Point, PointHash, PointEqual> otherRobotPos);

    static void initMapInfo(char map[Constancts::N][Constancts::N],
                            std::vector<std::vector<int>>& robotMap,
                            std::vector<std::vector<int>>& boatMap,
                            std::vector<PurchasePoint>& robotPurchasePt,
                            std::vector<PurchasePoint>& boatPurchasePt,
                            std::vector<DeliveryPoint>& deliveryPt);

    static void preprocessBerth(char map[Constancts::N][Constancts::N],
                                int berthNum,
                                std::vector<Berth>& berth,
                                std::vector<std::vector<int>> robotMap,
                                std::vector<std::vector<int>> boatMap);

    static void preprocessRobotPurchasePoint(std::vector<std::vector<int>> robotMap,
                                             std::vector<PurchasePoint>& robotPurchasePt);

    static void preprocessBoatPurchasePoint(std::vector<std::vector<int>> boatMap,
                                            std::vector<PurchasePoint>& boatPurchasePt);

    static void preprocessDeliveryPoint(std::vector<std::vector<int>> boatMap, std::vector<DeliveryPoint>& deliveryPt);

    static void preprocess(char map[Constancts::N][Constancts::N],
                           int berthNum,
                           std::vector<Berth>& berth,
                           std::vector<std::vector<int>>& robotMap,
                           std::vector<std::vector<int>>& boatMap,
                           std::vector<PurchasePoint>& robotPurchasePt,
                           std::vector<PurchasePoint>& boatPurchasePt,
                           std::vector<DeliveryPoint>& deliveryPt);

    static void
    preprocessGoods(std::vector<std::vector<int>> robotMap, std::vector<std::vector<bool>> speedUpMap, GoodInfo& gd);

    static std::pair<Point, int> reverseCalcNextPos(int boatDir, Point boatPt, int op);

    static std::pair<Point, int> calcNextPos(int boatDir, Point boatPt, int op);

    static bool isValidOfBoat(std::pair<Point, int> pd, std::vector<std::vector<int>> boatMap);
};