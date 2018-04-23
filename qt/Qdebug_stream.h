// From Jochen Ulrich. See
// http://lists.trolltech.com/qt-interest/2005-06/thread00166-0.html

#ifndef GRAPHENE_QDEBUG_STREAM_H
#define GRAPHENE_QDEBUG_STREAM_H


//== INCLUDES =================================================================


#include <iostream>
#include <streambuf>
#include <string>

#include <cstdio>

#include <QApplication>
#include <QTextEdit>
#include <QColor>


//== NAMESPACES ===============================================================


namespace graphene {
namespace qt {


//== CLASS DEFINITION =========================================================


class Qdebug_stream : public std::basic_streambuf<char>
{
public:

  Qdebug_stream(std::ostream &stream, QTextEdit* text_edit, QColor col) : m_stream(stream)
  {
    log_window = text_edit;
    m_old_buf = stream.rdbuf();
    stream.rdbuf(this);
    color = col;
  }

  ~Qdebug_stream()
  {
    // output anything that is left
    if (!m_string.empty())
    {
      QColor oldcol = log_window->textColor();
      log_window->setTextColor(color);
      log_window->append(m_string.c_str());
      log_window->setTextColor(oldcol);
    }

    m_stream.rdbuf(m_old_buf);
  }

protected:

  virtual int_type overflow(int_type v)
  {
    if (v == '\n')
    {
      QColor oldcol = log_window->textColor();
      log_window->setTextColor(color);
      log_window->append(m_string.c_str());
      log_window->setTextColor(oldcol);
      log_window->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
      qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

      printf("%s\n", m_string.c_str());      
      m_string.erase(m_string.begin(), m_string.end());
    }
    else
      m_string += v;

    return v;
  }

  virtual std::streamsize xsputn(const char *p, std::streamsize n)
  {
    m_string.append(p, p + n);

    size_t pos = 0;
    while (pos != std::string::npos)
    {
      pos = m_string.find('\n');
      if (pos != std::string::npos)
      {
        std::string tmp(m_string.begin(), m_string.begin() + pos);

        QColor oldcol = log_window->textColor();
        log_window->setTextColor(color);
        log_window->append(tmp.c_str());
        log_window->setTextColor(oldcol);
        log_window->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents);

        printf("%s\n", tmp.c_str());
        m_string.erase(m_string.begin(), m_string.begin() + pos + 1);
      }
    }

    return n;
  }


private:
  std::ostream &m_stream;
  std::streambuf *m_old_buf;
  std::string m_string;
  QTextEdit* log_window;
  QColor color;
};


//=============================================================================
} // namespace qt
} // namespace graphene
//=============================================================================
#endif // GRAPHENE_QDEBUG_STREAM_H
//=============================================================================
