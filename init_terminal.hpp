#ifndef INIT_TERMINAL_HPP
#define INIT_TERMINAL_HPP

//#include <ostream>
#include <iostream>
#include <sstream>
#include <string>

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

  inline void Draw_Header(int width) 
  {
    if (::terminal_color) 
    {
      std::string header = "Terminal linux by GxdTxnz";
      int header_length = header.length();
      int left_padding = (width - header_length) / 2;

      std::cout << Cursor_Position(1, left_padding);
      for (int i = 0; i < header_length; ++i) 
      {
      
        if (i % 2 == 0) 
        {
          std::cout << "/";
        } 

        else 
        {
          std::cout << "\\";
        }
      }
    
      std::cout << std::endl;
      std::cout << Cursor_Position(2, left_padding);
      std::cout << header << std::endl;
      std::cout << Cursor_Position(3, left_padding);
        
      for (int i = 0; i < header_length; ++i) 
      {
      
        if (i % 2 == 0) 
        {
          std::cout << "\\";
        }
      
        else 
        {
          std::cout << "/";
        }
      }
    }
  }

  inline void Draw_Window_Frame(int width, int height) 
  {
    
    if (::terminal_color) 
    {
      Draw_Header(width);
      
      for (int i = 1; i < height - 1; ++i) 
      {
        std::cout << Cursor_Position(i + 1, 1) << "|";
        std::cout << Cursor_Position(i + 1, width) << "|";
      }
      
      std::cout << Cursor_Position(height, 1);
      
      for (int i = 0; i < width; ++i) 
      {
        std::cout << "_";
      }
    }
  }
}

#endif
