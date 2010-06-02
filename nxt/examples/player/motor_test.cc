#include <libplayerc++/playerc++.h>
#include <unistd.h>

int main ( int argc, char *argv[] )
{
  using namespace PlayerCc;

  PlayerClient    robot ( "localhost" );
  Position1dProxy posB ( &robot, 0 );
  Position1dProxy posC ( &robot, 1 );

  for ( int i = 0; i <= 50; i++ )
    {
      robot.Read();
      posB.SetSpeed ( i / 100.0 );
      posC.SetSpeed ( -i / 100.0 );
      usleep ( 100000 );
    }
  for ( int i = 50; i >= -50; i-- )
    {
      robot.Read();
      posB.SetSpeed ( i / 100.0 );
      posC.SetSpeed ( -i / 100.0 );
      usleep ( 100000 );
    }
  for ( int i = -50; i <= 0; i++ )
    {
      robot.Read();
      posB.SetSpeed ( i / 100.0 );
      posC.SetSpeed ( -i / 100.0 );
      usleep ( 100000 );
    }
}

