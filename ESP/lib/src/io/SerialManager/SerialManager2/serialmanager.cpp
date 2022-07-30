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

SerialManager::SerialManager() : userErrorHandler(NULL), _serial(NULL), ManagerCount(0)
{
    clear();
}

void SerialManager::begin(Stream &serialPort)
{
    /* Save Serial Port configurations */
    _serial = &serialPort;
}

// This checks the Serial stream for characters, and assembles them into a buffer.
// When the terminator character (defined by EOL constant) is seen, it starts parsing the
// buffer for a prefix Manager, and calls handlers setup by addManager() method
void SerialManager::loop(void)
{
    char c;
    while (available() > 0)
    {
        c = read();
        bufferHandler(c);
    }
}

/* Clear buffer */
void SerialManager::clear(void)
{
    memset(buffer, 0, SERIAL_CMD_BUFF_LEN);
    pBuff = buffer;
}

/*
 * Send error response
 * NOTE: Will execute user defined callback (defined using addDefault method),
 * if no user defined callback it will send the ERROR message (sendERROR method).
 */
void SerialManager::error(void)
{
    if (NULL != userErrorHandler)
    {
        (*userErrorHandler)();
    }

    clear(); /* Clear buffer */
}

// Retrieve the next token ("word" or "argument") from the Manager buffer.
// returns a NULL if no more tokens exist.
char *SerialManager::next(void)
{
    return strtok_r(NULL, delimiters, &last);
}

void SerialManager::bufferHandler(char c)
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
            print("Received: ");
            println(buffer);
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
bool SerialManager::ManagerHandler(void)
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
        print("Token: \"");
        print(token);
        println("\"");
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
                println("- Match Found!");
#endif
                offset = (char *)(userInput + strlen(token));

                /* Check for query Manager */
                if (0 == strncmp(offset, "=?", 2))
                {
#if SERIAL_CMD_DBG_EN
                    println("Run test callback");
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
                    println("Run read callback");
#endif
                    /* Run read callback */
                    (*ManagerList[i].read)();
                }
                else if (('=' == *offset) && (NULL != *ManagerList[i].write))
                {
#if (SERIAL_CMD_DBG_EN == 1)
                    println("Run write callback");
#endif
                    /* Run write callback */
                    (*ManagerList[i].write)();
                }
                else if (NULL != *ManagerList[i].execute)
                {
#if SERIAL_CMD_DBG_EN
                    println("Run execute callback");
#endif
                    /* Run execute callback */
                    (*ManagerList[i].execute)();
                }
                else
                {
                    println("INVALID");
                    ret = false;
                    break;
                }

                ret = true;
                break;
            }

#if SERIAL_CMD_DBG_EN
            else
            {
                println("- Not a match!");
            }
#endif
        }
    }

    return ret;
}

// Adds a "Manager" and a handler function to the list of available Managers.
// This is used for matching a found token in the buffer, and gives the pointer
// to the handler function to deal with it.
void SerialManager::addManager(char *cmd, void (*test)(), void (*read)(), void (*write)(), void (*execute)())
{

#if SERIAL_CMD_DBG_EN
    print("[");
    print(ManagerCount);
    print("] New Manager: ");
    println(cmd);
#endif

    ManagerList = (serialManagerCallback *)realloc(ManagerList, (ManagerCount + 1) * sizeof(serialManagerCallback));
    strncpy(ManagerList[ManagerCount].Manager, cmd, SERIAL_CMD_BUFF_LEN);
    ManagerList[ManagerCount].test = test;
    ManagerList[ManagerCount].read = read;
    ManagerList[ManagerCount].write = write;
    ManagerList[ManagerCount].execute = execute;
    ManagerCount++;
}

/* Optional user-defined function to call when an error occurs, default is NULL */
void SerialManager::addError(void (*callback)())
{
    userErrorHandler = callback;
}

int SerialManager::available()
{
    int bytes = 0;
    if (NULL != _serial)
    {
        bytes = _serial->available();
    }
    return bytes;
}

int SerialManager::read()
{
    int bytes = 0;
    if (NULL != _serial)
    {
        bytes = _serial->read();
    }
    return bytes;
}

int SerialManager::peek()
{
    int bytes = 0;
    if (NULL != _serial)
    {
        bytes = _serial->peek();
    }
    return bytes;
}

void SerialManager::flush()
{
    if (NULL != _serial)
    {
        _serial->flush();
    }
}

size_t SerialManager::write(uint8_t x)
{
    (void)x;
    return 0;
}

SerialManager serialManager;