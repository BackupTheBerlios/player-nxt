 #include "nxtdc.hh"

using namespace std;

int main(void) {
    NXT::brick b;
    
    b.play_tone(880, 1000);

    b.msg_rate_check();

    return 0;
}
