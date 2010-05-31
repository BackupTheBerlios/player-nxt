#include <cerrno>
#include "chronos.hh"
#include <cstring>
#include <stdexcept>
#include <sys/time.h>

Chronos::Chronos ( double seconds_since_epoch )
{
  clock_ = seconds_since_epoch;
}

double Chronos::elapsed ( void ) const
  {
    return now() - clock_;
  }

void Chronos::reset ( void )
{
  clock_ = now();
}

double Chronos::now ( void )
  {
    struct timeval clock;

    if ( gettimeofday ( &clock, NULL ) != 0 )
      throw std::runtime_error ( strerror ( errno ) );

    return
      static_cast<double> ( clock.tv_sec ) +
      static_cast<double> ( clock.tv_usec ) * 1e-6;
  }
