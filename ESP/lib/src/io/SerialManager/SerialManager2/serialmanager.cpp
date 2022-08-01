#include "serialmanager.hpp"

#if SERIAL_CMD_DBG_EN

static void printHex(Stream &port, uint8_t *data, uint8_t length);
static void printHex(Stream &port, uint16_t *data, uint8_t length);

void printHex(Stream &port, uint8_t *data, uint8_t length) // prints 8-bit data in hex with leading zeroes
{
    for (int i = 0; i < length; i++)
    {
        // port.print("0x");
        if (data[i] < 0x10)
        {
            port.print("0");
        }
        port.print(data[i], HEX);
        port.print(" ");
    }
}

void printHex(Stream &port, uint16_t *data, uint8_t length) // prints 16-bit data in hex with leading zeroes
{
    for (int i = 0; i < length; i++)
    {
        // port.print("0x");
        uint8_t MSB = byte(data[i] >> 8);
        uint8_t LSB = byte(data[i]);
        if (MSB < 0x10)
        {
            port.print("0");
        }
        port.print(MSB, HEX);
        if (LSB < 0x10)
        {
            port.print("0");
        }
        port.print(LSB, HEX);
        port.print(" ");
    }
}
#endif

SerialManager2::SerialManager2() : userErrorHandler(NULL), _serial(NULL), ManagerCount(0), _serialManager2Active(false), newData(false)
{
    clear();
}

void SerialManager2::begin(Stream &serialPort)
{
    /* Save Serial Port configurations */
    _serial = &serialPort;
}

// This checks the Serial stream for characters, and assembles them into a buffer.
// When the terminator character (defined by EOL constant) is seen, it starts parsing the
// buffer for a prefix Manager, and calls handlers setup by addManager() method
void SerialManager2::loop(unsigned long timeout)
{
    log_d("Listening to serial");
    _serialManager2Active = true;
    Serial.setTimeout(timeout);
    static bool recvInProgress = false;
    char startDelimiter = '<'; //! we need to decide on a delimiter for the start of a message
    char endDelimiter = '>';   //! we need to decide on a delimiter for the end of a message
    char c;
    while ((available() > 0) && !newData)
    {
        c = read();
        if (recvInProgress)
        {
            if (c != endDelimiter)
            {
                bufferHandler(c);
            }
            else
            {
                recvInProgress = false;
                newData = true;
            }
        }
        else
        {
            if (c == startDelimiter)
            {
                recvInProgress = true;
            }
        }
    }
    delay(timeout);
    _serialManager2Active = false;
}

/* Clear buffer */
void SerialManager2::clear(void)
{
    memset(buffer, 0, SERIAL_CMD_BUFF_LEN);
    pBuff = buffer;
}

/*
 * Send error response
 * NOTE: Will execute user defined callback (defined using addDefault method),
 * if no user defined callback it will send the ERROR message (sendERROR method).
 */
void SerialManager2::error(void)
{
    if (NULL != userErrorHandler)
    {
        (*userErrorHandler)();
    }

    clear(); /* Clear buffer */
}

// Retrieve the next token ("word" or "argument") from the Manager buffer.
// returns a NULL if no more tokens exist.
char *SerialManager2::next(void)
{
    return strtok_r(NULL, delimiters, &last);
}

void SerialManager2::bufferHandler(char c)
{
    int len;
    char *lastChars = NULL;

    if ((pBuff - buffer) > (SERIAL_CMD_BUFF_LEN - 2)) /* Check buffer overflow */
    {
        error(); /* Send ERROR, Buffer overflow */
    }

    *pBuff++ = c;  /* Put character into buffer */
    *pBuff = '\0'; /* Always null terminate strings */

    if ((pBuff - buffer) > 2) /* Check buffer length */
    {
        /* Get EOL */
        len = strlen(buffer);
        lastChars = buffer + len - 2;

        /* Compare last chars to EOL */
        if (0 == strcmp(lastChars, EOL))
        {

            // *lastChars = '\0'; /* Replace EOL with NULL terminator */

#if (SERIAL_CMD_DBG_EN == 1)
            log_d("Received: %s", buffer);
#endif

            if (ManagerHandler())
            {
                clear();
            }
            else
            {
                error();
            }
        }
    }
}

/* Return true if match was found */
bool SerialManager2::ManagerHandler(void)
{
    int i;
    bool ret = false;
    char *token = NULL;
    char *offset = NULL;
    char userInput[SERIAL_CMD_BUFF_LEN];

    memcpy(userInput, buffer, SERIAL_CMD_BUFF_LEN);

    /* Search for Manager at start of buffer */
    token = strtok_r(buffer, delimiters, &last);

#if SERIAL_CMD_DBG_EN
    print("User input: (");
    printHex(Serial, (uint8_t *)userInput, SERIAL_CMD_BUFF_LEN);
    println(")");
#endif

    if (NULL != token)
    {

#if SERIAL_CMD_DBG_EN
        log_d("Token: %s", token);
#endif

        for (i = 0; (i < ManagerCount); i++)
        {

#if SERIAL_CMD_DBG_EN
            print("Case: \"");
            print(ManagerList[i].Manager);
            print("\" ");
#endif

            /* Compare the token against the list of known Managers */
            if (0 == strncmp(token, ManagerList[i].Manager, SERIAL_CMD_BUFF_LEN))
            {

#if SERIAL_CMD_DBG_EN
                log_d("- Match Found!");
#endif
                offset = (char *)(userInput + strlen(token));

                /* Check for query Manager */
                if (0 == strncmp(offset, "=?", 2))
                {
#if SERIAL_CMD_DBG_EN
                    log_d("Run test callback");
#endif
                    if (NULL != *ManagerList[i].test)
                    {
                        /* Run test callback */
                        (*ManagerList[i].test)();
                    }
                }
                else if (('?' == *offset) && (NULL != *ManagerList[i].read))
                {
#if SERIAL_CMD_DBG_EN
                    log_d("Run read callback");
#endif
                    /* Run read callback */
                    (*ManagerList[i].read)();
                }
                else if (('=' == *offset) && (NULL != *ManagerList[i].write))
                {
#if (SERIAL_CMD_DBG_EN == 1)
                    log_d("Run write callback");
#endif
                    /* Run write callback */
                    (*ManagerList[i].write)();
                }
                else if (NULL != *ManagerList[i].execute)
                {
#if SERIAL_CMD_DBG_EN
                    log_d("Run execute callback");
#endif
                    /* Run execute callback */
                    (*ManagerList[i].execute)();
                }
                else
                {
                    log_e("INVALID");
                    ret = false;
                    break;
                }

                ret = true;
                break;
            }

#if SERIAL_CMD_DBG_EN
            else
            {
                log_e("- Not a match!");
            }
#endif
        }
    }

    return ret;
}

// Adds a "Manager" and a handler function to the list of available Managers.
// This is used for matching a found token in the buffer, and gives the pointer
// to the handler function to deal with it.
void SerialManager2::addManager(char *cmd, void (*test)(), void (*read)(), void (*write)(), void (*execute)())
{

#if SERIAL_CMD_DBG_EN
    print("[");
    print(ManagerCount);
    print("] New Manager: ");
    println(cmd);
#endif

    ManagerList = (serialManager2Callback *)realloc(ManagerList, (ManagerCount + 1) * sizeof(serialManager2Callback));
    strncpy(ManagerList[ManagerCount].Manager, cmd, SERIAL_CMD_BUFF_LEN);
    ManagerList[ManagerCount].test = test;
    ManagerList[ManagerCount].read = read;
    ManagerList[ManagerCount].write = write;
    ManagerList[ManagerCount].execute = execute;
    ManagerCount++;
}

/* Optional user-defined function to call when an error occurs, default is NULL */
void SerialManager2::addError(void (*callback)())
{
    userErrorHandler = callback;
}

int SerialManager2::available()
{
    int bytes = 0;
    if (NULL != _serial)
    {
        bytes = _serial->available();
    }
    return bytes;
}

int SerialManager2::read()
{
    int bytes = 0;
    if (NULL != _serial)
    {
        bytes = _serial->read();
    }
    return bytes;
}

int SerialManager2::peek()
{
    int bytes = 0;
    if (NULL != _serial)
    {
        bytes = _serial->peek();
    }
    return bytes;
}

void SerialManager2::flush()
{
    if (NULL != _serial)
    {
        _serial->flush();
    }
}

size_t SerialManager2::write(uint8_t x)
{
    (void)x;
    return 0;
}

SerialManager2 serialManager2;