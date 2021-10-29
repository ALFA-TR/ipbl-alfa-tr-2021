#ifndef _speaker_h_
#define _speaker_h_

class I2SOutput;
class WAVFileReader;

class Speaker
{
private:
    WAVFileReader *m_ready_ping;
    I2SOutput *m_i2s_output;

public:
    Speaker(I2SOutput *i2s_output);
    ~Speaker();
    void playReady();
};

#endif