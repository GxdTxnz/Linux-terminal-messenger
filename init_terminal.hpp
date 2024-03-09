#ifndef INIT_TERMINAL_HPP
#define INIT_TERMINAL_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>

// Переменная для включения или отключения цветового вывода в терминале
extern bool terminal_color;

namespace terminal
{
  enum SGR 
  {
    RESET_ALL = 0,
    TEXT_BOLD = 1,
    TEXTCOLOR_BLACK = 30,
    TEXTCOLOR_RED = 31,
    TEXTCOLOR_GREEN = 32,
    TEXTCOLOR_YELLOW = 33,
    TEXTCOLOR_BLUE = 34,
    TEXTCOLOR_MAGENDA = 35,
    TEXTCOLOR_CYAN = 36,
    TEXTCOLOR_WHITE = 37,
    TEXTCOLOR_DEFAULT = 39,
  };

  inline std::ostream& operator<<( std::ostream& stream, SGR code ) 
  {
    
    if ( ::terminal_color ) 
    {
      return stream << "\x1B[" << static_cast<int>(code) << "m";
    } 
    
    else 
    {
      return stream;
    }
  }

  inline std::string Cursor_Previous_Line( int n = 1 ) 
  {
    
    if ( ::terminal_color ) 
    {
      std::ostringstream stringStream;
      stringStream << "\x1B[" << n << "F";
      return stringStream.str();
    } 

    else 
    {
      return "";
        
    }
  }
  
  inline std::string Cursor_Horizontal_Absolute( int n ) 
  {
  
    if ( ::terminal_color ) 
    {
      std::ostringstream stringStream;
      stringStream << "\x1B[" << n << "G";
      return stringStream.str();
    } 

    else 
    {
      return "";
    }
  }

  inline std::string Cursor_Position( int n, int m ) 
  {
    
    if ( ::terminal_color ) 
    {
      std::ostringstream stringStream;
      stringStream << "\x1B[" << n << ";" << m << "H";
      return stringStream.str();
    } 

    else 
    {
      return "";
    }
  }

  inline std::string Erase_Display( int n = 2 ) 
  {
    if ( ::terminal_color ) 
    {
      std::ostringstream stringStream;
      stringStream << "\x1B[" << n << "J";
      return stringStream.str();
    } 

    else 
    {
      return "";
    }
  }

  inline std::string Erase_Line( int n = 2 ) 
  {
    
    if ( ::terminal_color ) 
    {
      std::ostringstream stringStream;
      stringStream << "\x1B[" << n << "K";
      return stringStream.str();
    } 

    else 
    {
      return "";
    }
  }

  inline std::string Scroll_Up( int n = 1 ) 
  {
    if ( ::terminal_color ) 
    {
      std::ostringstream stringStream;
      stringStream << "\x1B[" << n << "S";
      return stringStream.str();
    } 

    else 
    {
      return "";
    }
  }

  inline std::string Scroll_Down( int n = 1 ) 
  {
    
    if ( ::terminal_color ) 
    {
      std::ostringstream stringStream;
      stringStream << "\x1B[" << n << "T";
      return stringStream.str();
    } 

    else 
    {
      return "";
    }
  }

  inline std::ostream& Save_Cursor_Position( std::ostream& stream ) 
  {
    
    if( ::terminal_color ) 
    {
      return stream << "\x1B[s";
    } 
    
    else 
    {
      return stream;
    }
  }

  inline std::ostream& Restore_Cursor_Position( std::ostream& stream ) 
  {
    
    if ( ::terminal_color ) 
    {
      return stream << "\x1B[u";
    } 

    else 
    {
      return stream;
    }
  }

  inline std::string Clear_Screen()
  {
    if ( ::terminal_color )
    {
      return "\x1B[2J\x1B[H"; // Очистка экрана и перемещение курсора в верхний левый угол
    }
    else
    {
      return "";
    }
  }

  inline std::string Center_Text(const std::string& text)
  {
    std::ostringstream stringStream;
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int width = w.ws_col;

    int padding = (width - text.length()) / 2;
    stringStream << std::string(padding, ' ') << text;
    return stringStream.str();
  }
  
  
  inline std::ostream& Print_Centered_Text(std::ostream& stream, const std::string& text)
  {
    if (::terminal_color)
    {
      struct winsize w;
      ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
      int width = w.ws_col;

      int padding = (width - text.length()) / 2;
      stream << std::string(padding, ' ') << text;
    }

    return stream;
  }
}

#endif
