#ifndef _chronos_
#define _chronos_

namespace driver_differential {

class Chronos
  {
  public:
    Chronos ( double seconds_since_epoch = now() );
    double elapsed ( void ) const;
    void reset ( void );

  private:
    double clock_;

    static double now ( void ) ;
  };

}

#endif
