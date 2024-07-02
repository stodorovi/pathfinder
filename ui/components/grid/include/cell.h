#pragma once

#include <QtWidgets/QTableWidgetItem>

enum class cell_state {
    empty,
    start,
    end,
    untraversable
};

inline cell_state toggle_state(cell_state current, cell_state to_set) {
    if (current == to_set) return cell_state::empty;
    return to_set;
}

class cell : public QTableWidgetItem {
private:
    cell_state _state = cell_state::empty;
    
public:
    cell_state state() const { return _state; }
    void state(cell_state s) {
        _state = toggle_state(_state, s);

        static auto to_color = [](cell_state cs) {
            switch (cs) {
                case cell_state::start:
                    return Qt::green;
                case cell_state::end:
                    return Qt::red;
                case cell_state::untraversable:
                    return Qt::black;
                case cell_state::empty:
                    return Qt::gray;
            };
            return Qt::gray;
        };

        setBackground(to_color(_state));
    }
};
