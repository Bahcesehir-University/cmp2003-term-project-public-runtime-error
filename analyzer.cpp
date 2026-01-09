#include "analyzer.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <cstddef>
#include <string_view>
#include <cctype>

using namespace std;

bool TripAnalyzer::cmpZone(const ZoneCount& a, const ZoneCount& b) {
    if (a.count != b.count) return a.count > b.count;
    return a.zone < b.zone;
}

bool TripAnalyzer::cmpSlot(const SlotCount& a, const SlotCount& b) {
    if (a.count != b.count) return a.count > b.count;
    if (a.zone != b.zone) return a.zone < b.zone;
    return a.hour < b.hour;
}

void TripAnalyzer::ingestFile(const std::string& csvPath) {
    ifstream file(csvPath);
    if (!file.is_open()) return;

    string row;
    bool isFirstRow = true;

    while (getline(file, row)) {
        if (row.empty()) continue;
        if (row.back() == '\r') row.pop_back();

        // Başlık satırı kontrolü: Eğer ilk karakter rakam değilse atla
        if (isFirstRow) {
            isFirstRow = false;
            if (!isdigit(row[0])) continue;
        }

        string_view v(row);
        size_t p1 = v.find(',');
        if (p1 == string_view::npos) continue;
        size_t p2 = v.find(',', p1 + 1);
        if (p2 == string_view::npos) continue;
        size_t p3 = v.find(',', p2 + 1);
        if (p3 == string_view::npos) continue;
        size_t p4 = v.find(',', p3 + 1);
        // p5 kontrolünü sildik çünkü her satırda 5. virgül olmayabilir!

        // pickupid ve pickuptime ayırma
        string pickupid(v.substr(p1 + 1, p2 - p1 - 1));
        
        // p4 bulunamazsa (satır p4'te bitiyorsa) substr hata vermesin diye kontrol
        string_view pickuptime;
        if (p4 == string_view::npos) 
            pickuptime = v.substr(p3 + 1);
        else 
            pickuptime = v.substr(p3 + 1, p4 - p3 - 1);

        // CASE SENSITIVITY FIX (B3 Testi için şart)
        for (auto& c : pickupid) c = toupper((unsigned char)c);

        if (pickuptime.length() < 13) continue;

        try {
            int hour = (pickuptime[11] - '0') * 10 + (pickuptime[12] - '0');
            if (hour >= 0 && hour <= 23) {
                zone_counter[pickupid]++;
                string combination = pickupid + "-" + to_string(hour);
                slot_counter[combination]++;
            }
        }
        catch (...) { continue; }
    }
    file.close();
}

std::vector<ZoneCount> TripAnalyzer::topZones(int k) const {
    vector<ZoneCount> zones;
    zones.reserve(zone_counter.size()); // HIZLANDIRMA
    for (const auto& z : zone_counter) {
        zones.push_back({ z.first, z.second });
    }
    int minimum = min(k, (int)zones.size());
    partial_sort(zones.begin(), zones.begin() + minimum, zones.end(), cmpZone);
    zones.resize(minimum);
    return zones;
}

std::vector<SlotCount> TripAnalyzer::topBusySlots(int k) const {
    vector<SlotCount> slots;
    slots.reserve(slot_counter.size()); // HIZLANDIRMA
    for (const auto& s : slot_counter) {
        size_t dash = s.first.find_last_of('-'); // Zone isminde '-' varsa hata olmasın diye last_of
        string zone = s.first.substr(0, dash);
        int hour = stoi(s.first.substr(dash + 1));
        slots.push_back({ zone, hour, s.second });
    }
    int minimum = min(k, (int)slots.size());
    partial_sort(slots.begin(), slots.begin() + minimum, slots.end(), cmpSlot);
    slots.resize(minimum);
    return slots;
}
