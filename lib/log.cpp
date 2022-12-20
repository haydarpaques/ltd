#include "log.h"

namespace ltd 
{
    namespace log 
    {

        const char* printf_formatter::read_adjustment_flag(std::ostream& out, const char* format)
        {
            if (*format == 0)
                return nullptr;

            if (*format == '-') 
            {
                format++;
                out.setf(std::ios::left);
            }
            else
                out.setf(std::ios::right);

            return format;
        }

        const char* printf_formatter::read_sign_flag(std::ostream& out, const char* format)
        {
            if (*format == 0)
                return nullptr;

            if (*format == '+')
            {
                format++;
                out.setf(std::ios::showpos);
            }
            else
                out.unsetf(std::ios::showpos);

            return format;
        }

        const char* printf_formatter::read_pound_flag(std::ostream& out, const char* format)
        {
            if (*format == 0)
                return nullptr;

            if (*format == '#') 
            {
                format++;
                pound_flag = true;
            }

            return format;
        }

        const char* printf_formatter::read_fill_flag(std::ostream& out, const char* format)
        {
            if (*format == 0)
                return nullptr;

            if (*format == '0')
            {
                format++;
                out.fill('0');
            }
                
            return format;
        }

        const char* printf_formatter::read_width(std::ostream& out, const char* format)
        {
            if (*format == 0)
                return nullptr;

            if (*format == '*') 
                expect_width = true;
            else if (std::isdigit(*format)) 
            {
                char buff[50];
                int  at = 0;

                do 
                {
                    buff[at++] = *format++;
                } while (std::isdigit(*format));

                buff[at] = 0;
                out.width(atoi(buff));
            }

            return format;
        }

        const char* printf_formatter::read_precision(std::ostream& out, const char* format)
        {
            if (*format == 0)
                return nullptr;

            if (*format == '.') {
                if (*++format == '*') {
                    expect_width = true;
                }
                else {
                    char buff[50];
                    int  at = 0;

                    while (std::isdigit(*format)) {
                        buff[at++] = *format++;
                    }

                    buff[at] = 0;

                    out.precision(atoi(buff));
                }
            }

            return format;
        }

        const char* printf_formatter::read_length(std::ostream& out, const char* format)
        {
            if (*format == 0)
                return nullptr;

            switch (*format) {
            case 'h':
                if (*++format == 'h') ++format;
                break;
            case 'l':
                if (*++format == 'l') ++format;
                break;
            case 'j':
            case 'z':
            case 't':
            case 'L':
                format++;
                break;
            default:
                return format;
            }
            return format;
        }

        const char* printf_formatter::read_specifier(std::ostream& out, const char* format)
        {
            if (*format == 0)
                return nullptr;

            switch (*format) {
            case 'o':
                format++;
                out.setf(std::ostream::oct);
                if (pound_flag)
                    out << '0';
                break;
            case 'x':
                format++;
                std::cout.setf(std::ios::hex, std::ios::basefield);
                if (pound_flag)
                    std::cout.setf(std::ios::showbase);
                break;
            case 'X':
                format++;
                out.setf(std::ostream::uppercase);
                std::cout.setf(std::ios::hex, std::ios::basefield);
                if (pound_flag)
                    std::cout.setf(std::ios::showbase);
                break;
            case 'F':
                out.setf(std::ostream::uppercase);
            case 'f':
                format++;
                out.setf(std::ostream::fixed);
                break;
            case 'E':
                out.setf(std::ostream::uppercase);
            case 'e':
                format++;
                std::cout.setf(std::ios::dec, std::ios::basefield);
                out.setf(std::ostream::scientific, std::ios_base::floatfield);
                break;
            case 'd':
            case 'i':
            case 'u':
                format++;
                out.setf(std::ostream::dec);
                break;
            case 's':
            case 'c':
                format++;
                break;
            default:
                return nullptr;
            }
            return format;
        }
    } // namespace log
} // namespace ltd