#ifndef _chronos_
#define _chronos_

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

#endif
