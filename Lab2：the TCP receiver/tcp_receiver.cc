#include "tcp_receiver.hh"

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    // TCP首部
    TCPHeader header = seg.header();
    // TCP数据
    Buffer payload = seg.payload();

    // 初始化isn
    if (header.syn) {
        _isn = header.seqno;
        _syn = true;
    }
    
    // 未建立连接，丢弃
    if (!_syn) {
        return;
    }

    // 写入数据
    uint64_t absolute_seqno = unwrap(header.seqno, _isn, stream_out().bytes_written() + 1);
    std::string data = string(payload.str());
    _reassembler.push_substring(data, absolute_seqno - (header.syn ? 0 : 1), header.fin);

    return;
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if (!_syn) {
        return nullopt;
    }
    // 当流真正关闭了，才需要+1
    return WrappingInt32(_isn + stream_out().bytes_written() + 1 + (stream_out().input_ended() ? 1 : 0));
}

size_t TCPReceiver::window_size() const { 
    return _capacity - stream_out().buffer_size();
}
