#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QString>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QCheckBox>
#include <QTextEdit>
#include <QMap>

class opApp: public QWidget {
private:
    QString base_dir;
    QString version = "0.1.0";
    QString input_tip = "<?php \necho \"hello word\";";
    // PHP5.6.1 C://qwec1//PHP5.6.1//bin//php.exe
    QMap<QString, QString> phps;

    // layout
    QVBoxLayout mainlayout;
    QHBoxLayout toplayout;
    QHBoxLayout bottomlayout;
    // widget
    QComboBox phpVersion;
    QPushButton reset;
    QPushButton run;
    QPushButton copy;
    QPushButton openfile;
    QPushButton savefile;
    QCheckBox setHtml;
    // text
    QTextEdit codeinput;
    QTextEdit output;


private:
    void initUI();
    void initEvents();
    void initPHP();
    // for reset
    void on_reset();
    void to_reset(bool state);

    void on_copy();
    void open_file();
    void save_file();
    void run_code();

    void set_icon(QString icon_path);
    QString get_output_tip();
public:
    opApp();
    ~opApp();
};

QStringList GetDirNameList(const QString &strDirpath);
QString execute_phpcode(QString php, QString temp_php);
#endif // WIDGET_H
