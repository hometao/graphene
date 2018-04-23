#ifndef SELECTION_PLUGIN_WIDGET_HH
#define SELECTION_PLUGIN_WIDGET_HH

#include <QWidget>
#include <QGroupBox>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QGridLayout>

namespace graphene {
namespace qt {

class Selection_plugin_widget : public QWidget
{
public:
    QVBoxLayout* vb;
    QVBoxLayout* vb2;
    QGroupBox* gb1;
    QRadioButton* cb1;
    QRadioButton* cb2;
    QRadioButton* cb3;
    QVBoxLayout* vb1;
    QGroupBox* gb2;

    QPushButton* pb1;
    QPushButton* pb2;
    QPushButton* pb3;
    QPushButton* pb4;
    QPushButton* pb5;
    QPushButton* pb6;
    QPushButton* pb7;

    QGridLayout* gl1;

public:
    void setupUi();
};

}
}

#endif // SELECTION_PLUGIN_WIDGET_HH
