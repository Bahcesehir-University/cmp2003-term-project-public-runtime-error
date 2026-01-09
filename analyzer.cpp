#include "analyzer.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <cstddef>
#include <string_view>


using namespace std;
bool TripAnalyzer::cmpZone(const ZoneCount& a, const ZoneCount& b) {
    if (a.count != b.count)
        return a.count > b.count;
    return a.zone < b.zone;
}
bool TripAnalyzer::cmpSlot(const SlotCount& a, const SlotCount& b) {
    if (a.count != b.count)
        return a.count > b.count;
    if (a.zone != b.zone)
        return a.zone < b.zone;
    return a.hour < b.hour;
}

// Students may use ANY data structure internally

void TripAnalyzer::ingestFile(const std::string& csvPath) {
    // TODO:
    // - open file
    // - skip header
    // - skip malformed rows
    // - extract PickupZoneID and pickup hour
    // - aggregate counts
    ifstream file(csvPath);
    if (file.is_open()) {
        string row;
        while (getline(file, row)) {
            if (row.empty()) {
                continue;
            }
            string_view v(row);
            size_t p1 = v.find(',');
            if (p1 == string_view::npos) continue;
            size_t p2 = v.find(',', p1 + 1);
            if (p2 == string_view::npos) continue;
            size_t p3 = v.find(',', p2 + 1);
			if (p3 == string_view::npos) continue;
			size_t p4 = v.find(',', p3 + 1);
			if (p4 == string_view::npos) continue;
			size_t p5 = v.find(',', p4 + 1);
			if (p5 == string_view::npos) continue;
            string pickupid(v.substr(p1 + 1, p2 - p1 - 1));
            string_view pickuptime = v.substr(p3 + 1, p4 - p3 - 1);
            if (pickuptime.length() < 13) continue;
            try {
                int hour = (pickuptime[11] - '0') * 10 + (pickuptime[12] - '0');
                if (hour >= 0 && hour <= 23) {
                    zone_counter[pickupid]++; // counting of zones 
                    string combination = pickupid + "-" + to_string(hour); // to combine id and hour
                    slot_counter[combination]++;   // counting of slots
                }
            }
            catch (...) {
                continue;
            }
        }
        file.close();

    }
    else {
        cout << "File cannot be found!" << endl;
    }
}

std::vector<ZoneCount> TripAnalyzer::topZones(int k) const {
    vector<ZoneCount> zones;
    for (const auto& z : zone_counter) {
        zones.push_back({ z.first,z.second });
    }
    int minimum = min(k, (int)zones.size());
    partial_sort(zones.begin(), zones.begin() + minimum, zones.end(), cmpZone);
    zones.resize(minimum);
    return zones;
    // TODO
    // - sort by count desc, zone asc
    // - return first k
}

std::vector<SlotCount> TripAnalyzer::topBusySlots(int k) const {
    vector<SlotCount> slots;
    for (const auto& s : slot_counter) {
        size_t find = s.first.find('-');
        string zone = s.first.substr(0, find);
        int hour = stoi(s.first.substr(find + 1));
        slots.push_back({ zone,hour,s.second });
    }
    int minimum = min(k, (int)slots.size());
    partial_sort(slots.begin(), slots.begin() + minimum, slots.end(), cmpSlot);
    slots.resize(minimum);
    // TODO
    // - sort by count desc, zone asc, hour asc
    // - return first k
    return slots;

}
