#ifndef _application_h_
#define _applicaiton_h_

#include "state_machine/States.h"

class I2SSampler;
class I2SOutput;
class State;
class Speaker;

class Application
{
private:
    State *m_detect_wake_word_state;
    State *m_current_state;

public:
    Application(I2SSampler *sample_provider, Speaker *speaker);
    ~Application();
    void run();
};

#endif