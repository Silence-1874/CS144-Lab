#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) : _map({}), _last(-1), _eof(false), _output(capacity), _capacity(capacity) {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    // 计算各个分界点的绝对索引
    const size_t first_unassembled =  _output.bytes_written();
    const size_t first_unacceptable = _output.bytes_read() + _capacity;

    // 将data逐字符插入_map，丢弃超出capacity的部分
    for (size_t i = 0; index + i < first_unacceptable && i < data.length(); i++) {
        _map.insert(pair<size_t, char>(i + index, data[i]));
    }

    // 装载新的可装载字符串
    string new_assembled;
    for (size_t i = first_unassembled; i < first_unacceptable && _map.count(i) == 1; i++) {
        new_assembled.append(1, _map[i]);
        _map.erase(i);
    }
    _output.write(new_assembled);

    if (eof) {
        // 标记已经出现过eof
        _eof = true;
        // 计算最后一个字符的绝对索引
        _last = index + data.length() - 1;
    }

    // 如果出现过eof，并且未装配的第一个字符绝对索引减1等于_last，说明已经全部装配了
    if (_eof && _output.bytes_written() - 1 == _last) {
        _output.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const {
    return _map.size();
}

bool StreamReassembler::empty() const {
    return unassembled_bytes() == 0;
}
