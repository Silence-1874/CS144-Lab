#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) :_map({}), _last(-1), _output(capacity), _capacity(capacity) {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    if (_map.count(_last) != 0) {
        _output.end_input();
        return;
    }
    // 计算各个分界点的绝对索引
    const size_t first_unassembled =  _output.bytes_written();
    const size_t first_unacceptable = _output.bytes_read() + _capacity;

    if (eof) {
        _last = index + data.length() - 1;
    }

    for (size_t i = index; i < data.size(); i++) {
        _map.insert(pair<size_t, char>(i, data[i]));
    }

    if (_map.count(first_unassembled) != 0) {
        string temp;
        for (size_t i = first_unassembled; i < first_unacceptable && _map.count(i) != 0; i++) {
            temp.append(reinterpret_cast<const char *>(_map[i]));
            _map.erase(i);
        }
        _output.write(temp);
    }
}

size_t StreamReassembler::unassembled_bytes() const {
    return _map.size();
}

bool StreamReassembler::empty() const {
    return unassembled_bytes() == 0;
}
