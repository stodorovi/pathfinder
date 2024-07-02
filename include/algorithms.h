#pragma once

#include <string>

enum class algorithm : size_t {
    dijkstra,
    a_star,
    d_star_lite,
    hpa_star
};

inline const std::string& to_string(algorithm a) {
    static const std::string algorithms[] {
        "Dijkstra",
        "A*",
        "D* Lite",
        "HPA*"
    };
    return algorithms[static_cast<size_t>(a)];
}
