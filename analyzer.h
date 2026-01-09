#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
struct ZoneCount {
    std::string zone;
    long long count;
};

struct SlotCount {
    std::string zone;
    int hour;              // 0–23
    long long count;
};

class TripAnalyzer {
public:
    // Parse Trips.csv, skip dirty rows, never crash
    void ingestFile(const std::string& csvPath);

    // Top K zones: count desc, zone asc
    std::vector<ZoneCount> topZones(int k = 10) const;
     

    // Top K slots: count desc, zone asc, hour asc
    std::vector<SlotCount> topBusySlots(int k = 10) const;

    static  bool cmpZone(const ZoneCount& a,const ZoneCount& b);   
    static bool cmpSlot(const SlotCount& a,const SlotCount& b);
private:
    std::unordered_map<std::string, long long> zone_counter;  //def for zone counter we defined here because we are gonna use them also in the other functions
    std::unordered_map<std::string, long long> slot_counter;  //def for slot counter
};
