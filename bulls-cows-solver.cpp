#include <algorithm>
#include <chrono>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

void fatal(string msg) {
    throw logic_error(msg);
}

int16_t len;
int16_t charset_size;
string charset;

vector<vector<bool>> slots;
vector<bool> slotd;
int16_t slotdc;

struct history_entry {
    vector<int16_t> t;
    int16_t a, b;
} h0;
vector<history_entry> history;

void init() {
    slots.resize(len);
    for (auto& slot : slots) {
        slot.resize(charset_size, true);
    }

    slotd.resize(len);

    h0.t.resize(len, -1);
}

bool is_t_valid() {
    for (auto const& h : history) {
        int16_t a = 0, b = 0;
        for (int16_t si = 0; si < len; si++) {
            if (h.t[si] == h0.t[si]) {
                a += 1;
            } else if (find(h0.t.begin(), h0.t.end(), h.t[si]) != h0.t.end()) {
                b += 1;
            }
        }
        if (a != h.a || b != h.b) {
            return false;
        }
    }
    return true;
}

void gen_t() {
    static vector<bool> used(charset_size);
    static int16_t si = 0;

RF:
    if (si < 0) {
        fatal("gen_t() failed");
    }

    int16_t ci = h0.t[si];
    if (ci >= 0) {
        used[ci] = false;
    }

    while (1) {
        if (++ci >= charset_size) {
            h0.t[si] = -1;
            si -= 1;
            goto RF;
        }
        if (!used[ci] && slots[si][ci]) {
            break;
        }
    }

    h0.t[si] = ci;
    used[ci] = true;
    if (si == len - 1) {
        if (is_t_valid()) {
            return;
        } else {
            goto RF;
        }
    } else {
        si += 1;
        goto RF;
    }
}

void result(int16_t a, int16_t b) {
    if (a == 0) {
        for (int16_t si = 0; si < len; si++) {
            slots[si][h0.t[si]] = false;
        }
    } else if (b == 0 && a == slotdc) {
        for (int16_t si = 0; si < len; si++) {
            if (!slotd[si]) {
                for (int16_t ti = 0; ti < len; ti++) {
                    slots[si][h0.t[ti]] = false;
                }
            }
        }
    } else if (a == len) {
        for (int16_t si = 0; si < len; si++) {
            for (int16_t ci = 0; ci < charset_size; ci++) {
                slots[si][ci] = h0.t[si] == ci;
            }
        }
    } else if (a + b == len) {
        for (int16_t si = 0; si < len; si++) {
            for (int16_t ci = 0; ci < charset_size; ci++) {
                if (count(h0.t.begin(), h0.t.end(), ci) == 0) {
                    slots[si][ci] = false;
                }
            }
        }
    }

    slotdc = 0;
    for (int16_t si = 0; si < len; si++) {
        int16_t c = 0;
        for (int16_t ci = 0; ci < charset_size; ci++) {
            c += slots[si][ci];
        }
        if (c == 1) {
            slotd[si] = true;
            slotdc += 1;
        }
    }
}

int main() {
    cout << "len: ";
    cin >> len;
    if (len == 0) {
        fatal("len == 0");
    }

    cout << "char set (default: \"0123456789\"): ";
    cin.get();
    getline(cin, charset);
    if (charset.empty()) {
        charset = "0123456789";
    }
    charset_size = charset.size();
    if (charset_size < len) {
        fatal("charset too small");
    }
    set<char> cs;
    for (char c : charset) {
        if (cs.count(c) > 0) {
            fatal("invalid charset");
        }
        cs.emplace(c);
    }
    sort(charset.begin(), charset.end());

    init();

    do {
        gen_t();

        for (int16_t ci : h0.t) {
            cout << charset[ci];
        }
        cout << " | [x] A [x] B: ";

        cin >> h0.a >> h0.b;
        result(h0.a, h0.b);

        history.push_back(h0);
    } while (slotdc < len);
}
