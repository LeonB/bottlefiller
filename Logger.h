#include <PrintEx.h>

#ifndef LOG
#define LOG

#define PGMT( pgm_ptr ) ( reinterpret_cast< const __FlashStringHelper * >( pgm_ptr ) )

class Logger: public PrintEx
{
public:
    Logger()
        : PrintEx(Serial)
    {
        Serial.println("Test");
    };

    /* void init(Print& in) */
    /* { */
    /* } */

    void error( const char *format, ... )
    {
        va_list vList;
        va_start( vList, format );
        _printf( format, vList );
        this->printf("\r\n", "");
        va_end( vList );
    }

    void error(const __FlashStringHelper *format, ...)
    {
        va_list vList;
        va_start( vList, format );
        _printf( format, strlen_P((const char*)format)+1, vList );
        this->printf("\r\n", "");
        va_end( vList );
    }

    void notice( const char *format, ... )
    {
        va_list vList;
        va_start( vList, format );
        _printf(format, vList );
        this->printf("\r\n", "");
        va_end( vList );
    }

    void notice(const __FlashStringHelper *format, ...)
    {
        va_list vList;
        va_start( vList, format );
        _printf( format, strlen_P((const char*)format)+1, vList );
        this->printf("\r\n", "");
        va_end( vList );
    }
};


extern Logger logger;

#endif
