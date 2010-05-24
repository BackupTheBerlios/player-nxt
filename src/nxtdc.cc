#include <cassert>
#include "nxtdc.hh"
#include <stdexcept>
#include <sys/time.h>
#include <typeinfo>

using namespace NXT;
using namespace std;

const uint16_t VENDOR_LEGO = 0x0694;
const uint16_t PRODUCT_NXT = 0x0002;

const int kNxtConfig    = 1;
const int kNxtInterface = 0;

enum direct_commands
    {
        command_play_tone = 0x03
    };

const char *usberr_to_str(int err)
{
    switch(err) {
        case 0:
            return "LIBUSB_SUCCESS";
        case -1:
            return "LIBUSB_ERROR_IO";
        case -2:
            return "LIBUSB_ERROR_INVALID_PARAM";
        case -3:
            return "LIBUSB_ERROR_ACCESS";
        case -4:
            return "LIBUSB_ERROR_NO_DEVICE";
        case -5:
            return "LIBUSB_ERROR_NOT_FOUND";
        case -6:
            return "LIBUSB_ERROR_BUSY";
        case -7:
            return "LIBUSB_ERROR_TIMEOUT";
        case -8:
            return "LIBUSB_ERROR_OVERFLOW";
        case -9:
            return "LIBUSB_ERROR_PIPE";
        case -10:
            return "LIBUSB_ERROR_INTERRUPTED";
        case -11:
            return "LIBUSB_ERROR_NO_MEM";
        case -12:
            return "LIBUSB_ERROR_NOT_SUPPORTED";
        case -99:
            return "LIBUSB_ERROR_OTHER";
        default:
            return "LIBUSB_ERROR_UNKNOWN";
    }
}

buffer & buffer::append_byte(uint8_t byte)
{
    push_back(static_cast<unsigned char>(byte));
    return *this;
}

buffer & buffer::append_word(uint16_t word)
{
    union
    {
        unsigned char bytes[2];
        uint16_t      le_word;
    } tmp;

    tmp.le_word = libusb_cpu_to_le16(word);

    push_back(tmp.bytes[0]);
    push_back(tmp.bytes[1]);

    return *this;
}

buffer & buffer::append(const buffer & buf)
{
    for (size_t i = 0; i < buf.size(); i++)
        push_back(buf[i]);

    return *this;
}

void USB_transport::usb_check(int usb_error)
{
    if (usb_error != LIBUSB_SUCCESS)
        throw runtime_error(string("USB error: ") + usberr_to_str(usb_error));
}

USB_transport::USB_transport(void)
{
    usb_check(libusb_init(&context_));
    libusb_set_debug(context_, 3);

    handle_ = libusb_open_device_with_vid_pid(context_, VENDOR_LEGO, PRODUCT_NXT);
    if (handle_ == NULL)
        throw runtime_error(string(typeid(*this).name()) + ": brick not found.");

    usb_check(libusb_set_configuration(handle_, kNxtConfig));
    usb_check(libusb_claim_interface(handle_, kNxtInterface));
    usb_check(libusb_reset_device(handle_));
}

USB_transport::~USB_transport(void)
{
    //    usb_check(libusb_release_interface(handle_, kNxtInterface));
    // Fails, why?

    libusb_close(handle_);
    libusb_exit(context_);
}

void USB_transport::write(const buffer &buf)
{
    int transferred;

    usb_check(libusb_bulk_transfer
      (handle_, 0x1,
      (unsigned char*)&buf[0], buf.size(),
      &transferred, 0));
    printf("T:%d\n", transferred);
}

const buffer & USB_transport::read(void)
{
    unsigned char buf[1001];
    int transferred;

    usb_check(libusb_bulk_transfer
      (handle_, 0x82,
      buf, 324,
      &transferred, 0));
    printf("%2x %2x %2x (%d read)\n", buf[0], buf[1], buf[2], transferred);

    return buffer();
}

brick::brick(void)
{
    ;
}

brick::~brick(void)
{
    ;
}

void brick::play_tone(uint16_t tone_Hz, uint16_t duration_ms)
{
    link_.write
      (
      buffer().
      append_byte(0x00).
      append_byte(command_play_tone).
      append_word(tone_Hz).
      append_word(duration_ms));

    link_.read();
}

void brick::fiemo(void)
{
    link_.write
      (
      buffer().
      append_byte(0x00). // direct with resp
      append_byte(0x04). //command
      append_byte(0x01). //all motors
      append_byte(50).   // 50% power
      append_byte(1).    // MODE_MOTOR_ON
      append_byte(1).    // REGULATION_MOTOR_SPEED
      append_byte(0).    // TURN_RATIO
      append_byte(0x20). // RUN_STATE_RUNNING
      append_word(0).append_word(0));// forever

    link_.read();
}

void brick::msg_rate_check(void)
{
    struct timeval start, now;
    int calls=0;

    assert(gettimeofday(&start, NULL) == 0);

    do {
        play_tone(440, 1);
        calls++;

        assert(gettimeofday(&now, NULL) == 0);
    } while ((double)start.tv_sec + (double)start.tv_usec/1000000.0 -
    (double)now.tv_sec - (double)now.tv_usec/1000000.0 > -10.0);

    printf("%d calls in 10s\n", calls);
}

const buffer & brick::assemble (telegram_types teltype, const buffer & payload)
{
    return
    buffer().
//    append_word(libusb_cpu_to_le16(1 + payload.size())).
    append_byte(teltype).
    append(payload);
}

