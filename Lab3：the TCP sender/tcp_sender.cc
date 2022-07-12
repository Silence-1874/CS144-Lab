#include "tcp_sender.hh"

#include "tcp_config.hh"

#include <random>

// Dummy implementation of a TCP sender

// For Lab 3, please replace with a real implementation that passes the
// automated checks run by `make check_lab3`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! \param[in] capacity the capacity of the outgoing byte stream
//! \param[in] retx_timeout the initial amount of time to wait before retransmitting the oldest outstanding segment
//! \param[in] fixed_isn the Initial Sequence Number to use, if set (otherwise uses a random ISN)
TCPSender::TCPSender(const size_t capacity, const uint16_t retx_timeout, const std::optional<WrappingInt32> fixed_isn)
    : _isn(fixed_isn.value_or(WrappingInt32{random_device()()}))
    , _initial_retransmission_timeout{retx_timeout}
    , _stream(capacity) 
    , _rto(retx_timeout)
    {}

uint64_t TCPSender::bytes_in_flight() const { return _bytes_in_flight; }

void TCPSender::fill_window() {
    TCPSegment segment;

    // 建立连接
    if (!_syn) {
        _syn = true;

        // 初始化序列号
        segment.header().seqno = _isn;
        // 设置SYN位
        segment.header().syn = true;

        _segments_out.push(segment);
        _outstanding_segment.push(segment);
        _next_seqno = unwrap(_isn + 1, _isn, _next_seqno);
        _bytes_in_flight = 1;

        _tick_on = true;
        return;
    }

    uint64_t _cur_window_size = _window_size == 0 ? 1 : _window_size;
    uint64_t capacity = _cur_window_size - _bytes_in_flight;
    // 创建并发送报文
    while (!_stream.buffer_empty() && capacity > 0) {
        // 写入数据载荷
        uint64_t len = min(TCPConfig::MAX_PAYLOAD_SIZE, capacity);
        segment.payload() = _stream.read(len);
        // 写入序列号
        segment.header().seqno = next_seqno();
        // 写入确认号
        segment.header().ackno = wrap(_ack, _isn);
        // 如果发送最后一个报文后有空间设置FIN，就设置
        if (!_fin && _stream.eof() && capacity > segment.length_in_sequence_space()) {
            _fin = true;
            segment.header().fin = true;
        }
        // 发送报文
        _segments_out.push(segment);
        // 如果报文携带数据
        if (segment.length_in_sequence_space() != 0) {
            // 更新_next_seqno
            _next_seqno += segment.length_in_sequence_space();
            // 开启计时器
            _tick_on = true;
            // 加入未确认队列
            _outstanding_segment.push(segment);
            // 更新_bytes_in_flight
            _bytes_in_flight = _next_seqno - _ack;
        }
        capacity = _cur_window_size - _bytes_in_flight;
    }

    capacity = _cur_window_size - _bytes_in_flight;
    // 发送空FIN报文
    if (!_fin && _stream.eof() && _cur_window_size > _bytes_in_flight) {
        _fin = true;
        segment.header().seqno = next_seqno();
        segment.header().ackno = wrap(_ack, _isn);
        segment.header().fin = true;

        _segments_out.push(segment);
        _next_seqno++;
        _outstanding_segment.push(segment);
        _bytes_in_flight++;
        _tick_on = true;
    }

}

//! \param ackno The remote receiver's ackno (acknowledgment number)
//! \param window_size The remote receiver's advertised window size
void TCPSender::ack_received(const WrappingInt32 ackno, const uint16_t window_size) {
    uint64_t absolute_ackno = unwrap(ackno, _isn, _next_seqno);

    // 收到已经确认的报文，丢弃
    if (absolute_ackno < _ack) {
        return;
    }

    // 收到还未发送的报文，丢弃
    if (absolute_ackno > _next_seqno) {
        _window_size = window_size;
        return;
    }

    // 没有未确认的报文，丢弃
    if (_outstanding_segment.empty()) {
        return;
    }

    // 累计确认，从_outstanding_segment中移除已完全确认的报文
    size_t len = _outstanding_segment.front().length_in_sequence_space();
    while (!_outstanding_segment.empty() &&  absolute_ackno - _ack >= len) {
        len = _outstanding_segment.front().length_in_sequence_space();
        // 更新_ack
        _ack += len;
        // 更新_bytes_in_flight
        _bytes_in_flight -= len;
        // 移出_outstanding_segment
        _outstanding_segment.pop();
        // 重置计时器
        _tick = 0;
    }
    // 更新窗口大小
    _window_size = window_size;
    // 窗口大小更新后，尝试发送数据
    fill_window();
    // 将RTO重置为初始值
    _rto = _initial_retransmission_timeout;
    // 如果未确认报文全部读完了，则关闭重传计时器
    if (_outstanding_segment.empty()) {
        _tick_on = false;
        _tick = 0;
    }
    // 将连续重传次数置为0
    _consecutive_retransmissions = 0;
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void TCPSender::tick(const size_t ms_since_last_tick) {
    _tick += ms_since_last_tick;
    // 重传计时器过期，并且存在可以重传的报文
    if (_tick >= _rto && !_outstanding_segment.empty()) {
        // 重传最早的未确认报文
        _segments_out.push(_outstanding_segment.front());
        // 如果窗口大小非0
        if (_window_size) {
            // 将连续重传数加1
            _consecutive_retransmissions++;
            // 将RTO扩大为原来的两倍
            _rto <<= 1;
        }
        // 重启重传计时器
        _tick = 0;
    }
}

unsigned int TCPSender::consecutive_retransmissions() const { return _consecutive_retransmissions; }

void TCPSender::send_empty_segment() {
    TCPSegment segment;
    segment.header().seqno = WrappingInt32(_next_seqno);
    _segments_out.push(segment);
}
