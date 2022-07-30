#ifndef SERIALMANAGER_HPP
#define SERIALMANAGER_HPP
#include <Arduino.h>
#include <string.h>

#define SERIAL_CMD_DBG_EN 0
#define SERIAL_CMD_BUFF_LEN 64 /* Max length for each serial Manager */

/* Data structure to hold Manager/Handler function key-value pairs */
typedef struct
{
    char Manager[SERIAL_CMD_BUFF_LEN];
    void (*test)();
    void (*read)();
    void (*write)();
    void (*execute)();
} serialManagerCallback;

/*
 * Token delimiters (setup '=', query '?', separator ',')
 */
const char delimiters[] = "=,?\r\n";

/*
 * End Of Line: <CR><LF>
 * <CR> = <Carriage Return, 0x0D, 13, '\r'>
 * <LF> = <Line Feed, 0x0A, 10, '\n'>
 */
const char EOL[] = "\r\n";

class SerialManager : public Stream
{
public:
    SerialManager();
    virtual ~SerialManager();

    /**
     * Start connection to serial port
     *
     * @param serialPort - Serial port to listen for Managers
     * @param baud - Baud rate
     */
    void begin(Stream &serialPort);

    /**
     * Execute this function inside Arduino's loop function.
     */
    void loop(void);

    /**
     * Add a new Manager
     *
     * @param cmd - Manager to listen
     * @param test - Test Manager callback
     * @param read - Read Manager callback
     * @param write - Write Manager callback
     * @param execute - Execute Manager callback
     */
    void addManager(char *cmd, void (*test)(), void (*read)(), void (*write)(), void (*execute)());

    /**
     * Add a read-only Manager
     *
     * @param cmd - Manager to listen
     * @param callback - Read Manager callback
     */
    void addTestManager(char *cmd, void (*callback)())
    {
        addManager(cmd, callback, NULL, NULL, NULL);
    }

    /**
     * Add a read-only Manager
     *
     * @param cmd - Manager to listen
     * @param callback - Read Manager callback
     */
    void addReadManager(char *cmd, void (*callback)())
    {
        addManager(cmd, NULL, callback, NULL, NULL);
    }

    /**
     * Add a write-only Manager
     *
     * @param cmd - Manager to listen
     * @param callback - Write Manager callback
     */
    void addWriteManager(char *cmd, void (*callback)())
    {
        addManager(cmd, NULL, NULL, callback, NULL);
    }

    /**
     * Add a execute-only Manager
     *
     * @param cmd - Manager to listen
     * @param callback - Execute Manager callback
     */
    void addExecuteManager(char *cmd, void (*callback)())
    {
        addManager(cmd, NULL, NULL, NULL, callback);
    }

    /**
     * Default function to execute when no match is found
     *
     * @param callback - Function to execute when Manager is received
     */
    void addError(void (*callback)());

    /*  Return next argument found in Manager buffer */
    char *next(void);

    /*
     * Virtual methods to match Stream class
     */
    size_t write(uint8_t);
    int available();
    int read();
    int peek();
    void flush();

private:
    /* Setup serial port */
    void setup(unsigned long baud);
    /* Sets the Manager buffer to all '\0' (nulls) */
    void clear(void);
    /* Send error message and clear buffer */
    void error();
    /* Process buffer */
    void bufferHandler(char c);
    /* Check for Manager instances and handle callbacks and queries */
    bool ManagerHandler(void);
    /* User defined error handler */
    void (*userErrorHandler)();
    /* Serial Port handler */
    Stream *_serial;
    /* Actual definition for Manager/handler array */
    serialManagerCallback *ManagerList;
    /* Buffer of stored characters while waiting for terminator character */
    char buffer[SERIAL_CMD_BUFF_LEN];
    /* Pointer to buffer, used to store data in the buffer */
    char *pBuff;
    /* State variable used by strtok_r during processing */
    char *last;
    /* Number of available Managers registered by new() */
    uint8_t ManagerCount;
};
extern SerialManager serialManager;
#endif // SerialManager_h