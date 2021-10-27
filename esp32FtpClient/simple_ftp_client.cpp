/** @file simple_ftp_client.cpp
    @brief Function implimentation for ftp client library.

    This contains the implimentation for the ftp client library
    functionalities.

    @author Dhananjay Khairnar
*/
#include "simple_ftp_client.h"
#include "SD_card_helper.h"

WiFiClient client;
WiFiClient dataclient;
WiFiClient ftpClient;

String recvBuf;
char ftpRespDataBuf[100] = {0};

/** @brief flush receive buffer

    @return void
*/
void ftpflush()
{
  recvBuf = "";
  for (int i = 0 ; i < 50 ; i++)
  {
    while (ftpClient.available())
    {
      char ch = ftpClient.read();
      ch = '0';
    }
    delay(1);
  }
}

/** @brief establis connection to ftp server

    @return true if success / false if  failed
*/
bool connectToFTPServer()
{
  if (!ftpClient.connect(host, ftpPort)) {
    FTPLogLn("FTP Server connection failed");
    return false;
  }
  readFTPResponse();
  return true;
}

/** @brief This function read responce comming from ftp server

    read responses from ftp server and stores it in buffre \a recvBuf

    @return void
*/
void readFTPResponse()
{  
  String temp;
  char ch;
  for (int i = 0 ; i < 100 ; i++)
  {
    while (ftpClient.available())
    {
      ch = ftpClient.read();
      recvBuf += ch;
    }
    delay(1);
  }
}

/** @brief This function sends command to ftp server

    sends command to ftp server and get responce from ftp server,
     then it parse that respoce to get responce code.

    @param cmd ftp command

    @return ftp responce code
*/
uint8_t sendFTPCommand(String cmd)
{
  int ret = 0;
  FTPLog("< ");
  FTPLogLn(cmd);
  ftpflush();
  ftpClient.flush();
  ftpClient.println(cmd);
  delay(DELAY_BETWEEN_COMMAND);
  volatile unsigned long t = millis();
  while ( ((millis() - t) < COMMAND_TIME_OUT) && (recvBuf.length() < 3))
    readFTPResponse();
  FTPLog("> ");
  FTPLogLn(recvBuf); 
  if (recvBuf.length())
    sscanf(recvBuf.c_str(), "%d ", &ret);
  char *ptr;
  ptr = (char*)recvBuf.c_str();
  while (*ptr)
  {
    if (isdigit (*ptr) ) {
      ret = strtoul (ptr, &ptr, 10);
      break;
    }
    ptr++;
  }

  return (uint8_t)ret;
}

/** @brief This function sends login sequence to ftp serce

    sends login sequeto ftp server and return status of login

    @param username username for login
    @param password password for login

    @return if login success then true / if login fails then false
*/
bool loginFTPServer(String username , String password)
{
  volatile uint8_t ret = 0;
  volatile uint8_t retry = LOGIN_RETRY;
  delay(50);
  while (retry > 0)
  {
    ftpflush();
    ret = sendFTPCommand("USER " + username);
    delay(50);
    ftpflush();
    ret = sendFTPCommand("PASS " + password);
    if ( (LOGIN_SUCCESS == ret) | (ALREADY_LOGIN == ret) )
    {
      return true;
    }
    retry--;
  }
  return false;
}

/** @brief This function parse data port send by ftp server

    parse data port used for transfer data from ftp server and stores it in \a DataPort

    @return if parse success then true / if parse fails then false
*/
bool parseFTPDataPort()
{
  DataPort = 0;
  char *ptr;
  ptr = (char*)recvBuf.c_str();
  ptr += 3;
  while (*ptr)
  {
    if (isdigit (*ptr) ) {
      DataPort = strtoul (ptr, &ptr, 10);
    }
    ptr++;
  }

  FTPLogLn(String("Port Parsed ") + DataPort);
  if (DataPort > 0)
  {
    return true;
  }
  return false;
}

/** @brief This function initialize file download sequence

    @return if success then true / if fails then false
*/
bool initFileDownloadSequence()
{
  sendFTPCommand("NOOP");
  sendFTPCommand("TYPE I");
  sendFTPCommand("TYPE I");
  delay(200);
  sendFTPCommand("EPSV");
  return parseFTPDataPort();
}

/** @brief This function download file from ftp server(Using socket api)

    function download file from ftp server. before calling this function you have to
    connect to ftp saerver then login using connectToFTPServer() and loginFTPServer() functions

    @param path path of file on ftp folder

    @return true if success / false if fails
*/
bool downloadFileFromFTP(String path)
{
  int socket_fd;
  struct sockaddr_in ra;
  File file;
  int recvDataCount = 0;
  uint8_t data_buffer[5000];

  if (!initFileDownloadSequence())
  {
    return false;
  }

  socket_fd = socket(PF_INET, SOCK_STREAM, 0);
  if ( socket_fd < 0 )
  {
    printf("socket call failed");
    return false;
  }

  /* Receiver connects to server ip-address. */

  memset(&ra, 0, sizeof(struct sockaddr_in));
  ra.sin_family = AF_INET;
  ra.sin_addr.s_addr = inet_addr(host);
  ra.sin_port = htons(DataPort);

  if (connect(socket_fd, (const sockaddr*)&ra, sizeof(struct sockaddr_in)) < 0)
  {
    printf("connect failed \n");
    close(socket_fd);
    return false;
  }

  int recvCount = 0;

  if (OPENING_FILE == sendFTPCommand("RETR " + path))
  {
    SD_rmFile(path.c_str());
    file = SD.open(path, FILE_WRITE);
    if (!file) {
      FTPLogLn("Failed to open file for writing");
      close(socket_fd);
      return false;
    }
  }
  else
  {
    close(socket_fd);
    return false;
  }

  FTPLogLn("Downloading File, Please Wait.");
  uint32_t t = millis();
  while ( (recvDataCount = recv(socket_fd, data_buffer, sizeof(data_buffer), 0)) > 0)
  {
    file.write(data_buffer, recvDataCount);
    recvCount += recvDataCount;
  }
  uint32_t ttotal = (millis() - t);
  Serial.printf("Download Completed, Time required %d ms, File Size %d bytes\n\n" , ttotal , recvCount);
  file.close();
  close(socket_fd);
  return true;
}

/** @brief This function download file from ftp server (Using wifi client api)

    function download file from ftp server. before calling this function you have to
    connect to ftp saerver then login using connectToFTPServer() and loginFTPServer() functions

    @param path path of file on ftp folder

    @return true if success / false if fails
*/
bool getFile(String path)
{
  char ch;
  recvBuf = "";
  File file;

  unsigned int i = 0, j = 0;
  uint8_t dataBuf[4096];

  initFileDownloadSequence();

  client.connect(host, DataPort);

  if (OPENING_FILE == sendFTPCommand("RETR " + path))
  {
    SD_rmFile(path.c_str());
    file = SD.open(path, FILE_WRITE);
    if (!file) {
      FTPLogLn("Failed to open file for writing");
      return false;
    }
  }

  client.flush();
  uint32_t t = millis();
  while (client.connected())
  {
    while (client.available() > 0)
    {
      j = client.read(dataBuf, 4096);
      file.write(dataBuf, j);
      i += j;
    }
  }

  uint32_t ttotal = (millis() - t);
  delay(2);
  FTPLogLn("File Download Completed");
  FTPLogLn(String("Time required Download File ") + (ttotal / 1000) + "sec File Size " + i + "kb " + " Speed of file transfer " + ((i / 1024) / (ttotal / 1000)) + " kbps");
  client.stop();
  file.close();
  return true;
}
