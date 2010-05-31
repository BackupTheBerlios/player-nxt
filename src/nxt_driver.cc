/*
 *  Player - One Hell of a Robot Server
 *  Copyright (C) 2007
 *     Brian Gerkey
 *
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/** @ingroup drivers */
/** @{ */
/** @defgroup driver_nxt nxt
 * @brief Lego Mindstorms NXT

This driver implements partial interaction with a USB-connected Lego Mindstorms NXT brick.\n
Motors are implemented.\n
Sensors are unimplemented.

@par Compile-time dependencies

- libusb-1.0 or newer (www.libusb.org)

@par Provides

- @ref interface_position1d
    - One per each of the A, B, C motors
    - These can be aggregated in a position2d using, e.g., @ref driver_differential
    - Velocity commands are accepted. Position commands are not.
- @ref interface_power
    - Battery level of the brick.

@par Configuration file options

- max_power (float [%] default: 100)
  - Power applied when maximum vel is requested.

- max_speed (float [length/s] default: 0.5)
  - Speed that the motor provides (must be calibrated/measured somehow depending on the LEGO model built).

- period (float [s] default 0.05)
  - Seconds between reads of motor encoders. Since this requires polling and affects CPU use, each app can set an adequate timing.
  - Note that a polling roundtrip via USB takes (empirically measured) around 2ms per motor.

@par Example

@verbatim

# Standard configured brick with B and C motors in use

unit_length "m"
unit_angle  "radians"

# The NXT driver
driver
(
  name "nxt"
  provides [ "B:::position1d:0" "C:::position1d:0" "power:0" ]

  max_power 100 # 100% power is to be used
  max_speed 0.5 # in order to achieve 0.5 m/s linearly
)

# The differential driver that provides simplified position2d management
driver
(
  name "differential"
  requires [ "left:::position1d:0" "right:::position1d:0" ]
  provides [ "position2d:0" ]

  axis_length 0.5
)

# Needed renaming of interfaces to properly match them:
driver
(
  name "passthrough"
  requires [ "B:::position1d:0" ]
  provides [ "left:::position1d:0" ]
)

driver
(
  name "passthrough"
  requires [ "C:::position1d:0" ]
  provides [ "right:::position1d:0" ]
)

@endverbatim

@author Alejandro R. Mosteo
*/
/** @} */

#include "chronos.hh"
#include "libplayercore/device.h"
#include "libplayercore/driver.h"
#include "libplayercore/playercore.h"
#include "nxtdc.hh"

class Nxt : public ThreadedDriver
  {
  public:
    Nxt ( ConfigFile* cf, int section );

    virtual void Main ( void );
    virtual int  MainSetup ( void );
    virtual void MainQuit ( void );

    virtual int ProcessMessage ( QueuePointer &resp_queue, player_msghdr * hdr, void * data );

  private:
    player_devaddr_t motor_addr_[3];
    player_devaddr_t power_addr_;

    bool             publish_motor_[3];
    bool             publish_power_;

    double           period_;
    Chronos          timer_battery_;
    Chronos          timer_period_;

    NXT::brick       *brick_;

    void             CheckBattery ( void );
  };

Driver* nxt_Init ( ConfigFile* cf, int section )
{
  return static_cast<Driver*> ( new Nxt ( cf, section ) );
}

void nxt_Register ( DriverTable* table )
{
  table->AddDriver ( "nxt", nxt_Init );
}

Nxt::Nxt ( ConfigFile *cf, int section )
    : ThreadedDriver ( cf, section ),
    period_ ( cf->ReadFloat ( section, "period", 0.05 ) ),
    timer_battery_ ( -666.0 )   // Ensure first update to be sent immediately
{
  const char *motor_names[] = { "A", "B", "C" };

  if ( cf->ReadDeviceAddr ( &power_addr_, section, "provides", PLAYER_POWER_CODE, -1, NULL ) == 0 )
    {
      if ( AddInterface ( power_addr_ ) != 0 )
        throw std::runtime_error ( "Cannot add power interface" );
      else
        publish_power_ = true;
    }
  else
    {
      publish_power_ = false;
    }

}

int Nxt::MainSetup ( void )
{
  brick_ = new NXT::brick();
  return 0;
}

void Nxt::MainQuit ( void )
{
  delete brick_;
}

void Nxt::Main ( void )
{
  while ( true )
    {
      // Wait till we get new data or we need to measure something
      Wait ( period_ );

      pthread_testcancel();

      ProcessMessages ( 0 );

      CheckBattery();
    }
}

void Nxt::CheckBattery ( void )
{
  printf ( "1\n" );
  if ( ! publish_power_ )
    return;
  printf ( "2\n" );

  // We don't want to poll battery level innecesarily often
  if ( timer_battery_.elapsed() < 60.0 )
    return;
  printf ( "3\n" );

  timer_battery_.reset();
  printf ( "4\n" );

  player_power_data_t juice =
  {
    PLAYER_POWER_MASK_VOLTS,
    static_cast<float> ( brick_->get_battery_level() ) / 1000.0f
    // Omitted 4 unknown values here
  };
  printf ( "5\n" );


  // Publish value
  Publish ( power_addr_,
            PLAYER_MSGTYPE_DATA,
            PLAYER_POWER_DATA_STATE,
            static_cast<void*> ( &juice ) );

  printf ( "Publishing power: %8.2f\n", juice.volts );
}

int Nxt::ProcessMessage ( QueuePointer  & resp_queue,
                          player_msghdr * hdr,
                          void          * data )
{
  printf ( "nxt: Message not processed!\n" );
  return -1;
}
