#include <libusb-1.0/libusb.h>
#include <stdexcept>
#include <vector>

namespace NXT
  {

  using namespace std;

  typedef vector<unsigned char> protobuffer;

  class buffer : public protobuffer
    {
    public:
      buffer & append_byte ( uint8_t byte );
      buffer & append_word ( uint16_t word );
      buffer & append ( const buffer & buf );
      // return self to chain calls
    };

  class transport
    {
    public:
      virtual void write ( const buffer &buf ) = 0;
      virtual buffer read ( void ) = 0;
    };

  class USB_transport : public transport
    {
    public:
      USB_transport ( void );
      ~USB_transport ( void );
      virtual void write ( const buffer &buf );
      virtual buffer read ( void );
    private:
      libusb_context *context_;
      libusb_device_handle *handle_;

      void usb_check ( int usb_error );
    };

  class nxt_error : public runtime_error {};

  enum motors { A = 0x00, B = 0x01, C = 0x02, All = 0xFF };

  class brick
    {

    public:

      brick ( void );
      // Connect to the first brick found
      // TODO: give some means of connecting to a particular brick.
      // As is, this library serves only for using with a single brick.

      ~brick ( void );

      void execute ( const buffer &command, bool with_feedback = false );
      // Execute a prepared command
      // When with_feedback, the brick is asked to confirm proper execution

      // PREPARED COMMANDS
      // That you an store and execute with or without feedback

      buffer prepare_play_tone ( uint16_t tone_Hz, uint16_t duration_ms );

      // DIRECT PERFORMING (WITH FEEDBACK)
      // If you don't want the feedback overhead, use execute with prepared commands
      // Errors will be reported as thrown nxt_error

      void play_tone ( uint16_t tone_Hz, uint16_t duration_ms );

      void set_motor ( motors motor, int8_t power, uint32_t duration_ms );

      void msg_rate_check ( void );
      //  Run a 10-second loop of play_tone, to get the average time per commands
      //  For testing purposes, yes.

    private:

      enum telegram_types
      {
        direct_command_with_response    = 0x00,
        system_command_with_response    = 0x01,
        reply                           = 0x02,
        direct_command_without_response = 0x80,
        system_command_without_response = 0x80
      };

      const buffer & assemble ( telegram_types teltype,
                                uint8_t        command,
                                const buffer & payload );
      //  Assembles the full telegram to be sent over usb or bluetooth.

      USB_transport link_;
      //  For now is a fixed USB transport, but we could easily add bluetooth here

    };

}
