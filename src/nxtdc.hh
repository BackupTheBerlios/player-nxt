#include <libusb-1.0/libusb.h>
#include <vector>

namespace NXT {

    using namespace std;

    enum telegram_types
        {
            direct_command_with_response    = 0x00,
            system_command_with_response    = 0x01,
            reply                           = 0x02,
            direct_command_without_response = 0x80,
            system_command_without_response = 0x80
        };

    typedef vector<unsigned char> protobuffer;

    class buffer : public protobuffer {
        public:
        buffer & append_byte(uint8_t byte);
        buffer & append_word(uint16_t word);
        buffer & append(const buffer & buf);
        // return self to chain calls
    };

    class transport {
    public:
        virtual void write(const buffer &buf) = 0;
        virtual const buffer & read(void) = 0;
    };

    class USB_transport : public transport {
    public:
        USB_transport(void);
        ~USB_transport(void);
        virtual void write(const buffer &buf);
        virtual const buffer & read(void);
    private:
        libusb_context *context_;
        libusb_device_handle *handle_;

        void usb_check(int usb_error);
    };

    class brick {

    public:

        brick(void);
        // Connect to the first brick found
        // TODO: give some means of connecting to a particular brick.
        // As is, this library serves only to use a single brick.

        ~brick(void);

        void play_tone(uint16_t tone_Hz, uint16_t duration_ms);
        void fiemo(void);
        void msg_rate_check(void);

    private:

        const buffer & assemble (telegram_types teltype, const buffer & payload);
        //  Assembles the full telegram to be sent over usb or bluetooth.
        //  Payload must be the specific telegram data; size is automatically
        //    derived.

        USB_transport link_;

    };

}
