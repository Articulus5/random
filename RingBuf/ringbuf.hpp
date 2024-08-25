#ifndef RINGBUF_HPP
#define RINGBUF_HPP

class RingBuffer {
public:
     RingBuffer(void);
     RingBuffer(unsigned int buf_size);
     ~RingBuffer(void);
     unsigned int Put(unsigned char *data, unsigned int data_size);
     unsigned int Get(unsigned char *data, unsigned int data_size);

protected:
     bool IsEmpty(void);
     bool IsFull(void);
     unsigned int Usage(void);

private:
     unsigned int size;
     unsigned int first;
     unsigned int last;
     unsigned char *buffer;
};

#endif // RINGBUF_HPP
