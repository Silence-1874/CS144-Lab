#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity) : _buf_capacity(capacity){}

size_t ByteStream::write(const string &data) {
    const size_t ret = min(data.size(), remaining_capacity());
    _total_written += ret;
    for (size_t i = 0; i < ret; i++) {
        _buf.push_back(data[i]);
    }
    return ret;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    const size_t peek_len = min(len, buffer_size());
    return string().assign(_buf.begin(), _buf.begin() + peek_len);
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    const size_t pop_len = min(len, _buf.size());
    _total_read += pop_len;
    for (size_t i = 0; i < pop_len; i++) {
        _buf.pop_front();
    }
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    const string ret = peek_output(len);
    pop_output(len);
    return ret;
}

void ByteStream::end_input() {
    _end_input = true;
}

bool ByteStream::input_ended() const {
    return _end_input;
}

size_t ByteStream::buffer_size() const {
    return _buf.size();
}

bool ByteStream::buffer_empty() const {
    return _buf.empty();
}

bool ByteStream::eof() const {
    return input_ended() && buffer_empty();
}

size_t ByteStream::bytes_written() const {
    return _total_written;
}

size_t ByteStream::bytes_read() const {
    return _total_read;
}

size_t ByteStream::remaining_capacity() const {
    return _buf_capacity - _buf.size();
}
