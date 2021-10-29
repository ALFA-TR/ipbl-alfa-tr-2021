#include "Speaker.h"
#include "I2SOutput.h"
#include "WAVFileReader.h"

Speaker::Speaker(I2SOutput *i2s_output)
{
    m_ready_ping = new WAVFileReader("/ready_ping.wav");
}

Speaker::~Speaker()
{
    delete m_ready_ping;
}

void Speaker::playReady()
{
    m_ready_ping->reset();
    m_i2s_output->setSampleGenerator(m_ready_ping);
}