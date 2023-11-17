#include "widget.h"
#include <QMessageBox>
#include <QObject>
#include <QFont>
#include <QApplication>
#include <QClipboard>
#include <QFileDialog>
#include <QByteArray>
#include <QProcess>

#include <iostream>
using std::cout, std::endl;

opApp::opApp() {
    this->base_dir = QCoreApplication::applicationDirPath();
    this->initPHP();
    this->initUI();
    this->initEvents();
}

opApp::~opApp() {

}


void opApp::initUI() {
    this->setGeometry(300, 300, 1000, 600);
    this->setWindowTitle("offPHP v" + this->version);

    // init layout
    this->setLayout(&this->mainlayout);
    this->mainlayout.addLayout(&this->toplayout);
    this->mainlayout.addLayout(&this->bottomlayout);

    // buttons
    this->reset.setText("Reset");
    this->run.setText("&Run");
    this->copy.setText("&Copy");
    this->openfile.setText("&Open");
    this->savefile.setText("&Save");
    this->setHtml.setText("Set Html");

    // TextEdit
    this->codeinput.setLineWrapMode(QTextEdit::WidgetWidth);
    this->codeinput.setAcceptRichText(false);
    this->codeinput.setFont(QFont("Consolas", 14));
    this->codeinput.setPlainText(this->input_tip);

    this->output.setLineWrapMode(QTextEdit::WidgetWidth);
    this->output.setReadOnly(true);
    this->output.setFont(QFont("Consolas", 10));
    this->output.setPlaceholderText(this->get_output_tip());

    // toplayout widgets
    this->toplayout.addWidget(&this->phpVersion);
    this->toplayout.addWidget(&this->reset);
    this->toplayout.addWidget(&this->run);
    this->toplayout.addWidget(&this->copy);
    this->toplayout.addWidget(&this->openfile);
    this->toplayout.addWidget(&this->savefile);
    this->toplayout.addWidget(&this->setHtml);

    // bottom widgets
    this->bottomlayout.addWidget(&this->codeinput);
    this->bottomlayout.addWidget(&this->output);

    this->set_icon("op.ico");
}

void opApp::initEvents() {
    QObject::connect(&this->reset, &QPushButton::clicked,this, &opApp::on_reset);
    QObject::connect(&this->copy, &QPushButton::clicked, this, &opApp::on_copy);
    QObject::connect(&this->openfile, &QPushButton::clicked, this, &opApp::open_file);
    QObject::connect(&this->savefile, &QPushButton::clicked, this, &opApp::save_file);
    QObject::connect(&this->run, &QPushButton::clicked, this, &opApp::run_code);

}

void opApp::initPHP() {
    QString php_dir = this->base_dir + "/phps";
    QStringList all_php = GetDirNameList(php_dir);

    if (all_php.length() == 0 ){
        QMessageBox::warning(this, "Please check the php directory", "Please check the php directory");
        exit(-1);
    }

    for (QString phpversion: all_php) {
        QString php_exe = php_dir + "/" + phpversion +"/php.exe";
        this->phps.insert(phpversion, php_exe);
        cout << php_exe.toStdString() << endl;
    }

    this->phpVersion.addItems(all_php);

}

void opApp::on_reset() {
    QMessageBox::StandardButton res = QMessageBox::question(this, "Reset", "Do you want to reset?",
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::Yes
    );

    if (res == QMessageBox::Yes) {
        this->to_reset(true);
    } else {
        this->to_reset(false);
    }

}

void opApp::to_reset(bool state) {
    if (state){
        this->codeinput.setPlainText("<?php \necho \"hello word\";");
        this->output.clear();
    }

}

void opApp::on_copy() {
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(this->codeinput.toPlainText());
    QMessageBox::information(this, "Copy code success","Copy success");

}

void opApp::open_file() {
    QString filename = QFileDialog::getOpenFileName(this, "Open File", "", "Code Files (*.*)");
    QFile f(filename);
    f.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray content = f.readAll();
    this->codeinput.setPlainText(content);

}

void opApp::save_file() {
    QString filename = QFileDialog::getSaveFileName(this, "Save Fole", "", "Code Files (*.php)");
    QFile f(filename);
    f.open(QIODevice::WriteOnly | QIODevice::Text);
    QString content = this->codeinput.toPlainText();
    f.write(content.toStdString().c_str());

}

void opApp::set_icon(QString icon_path){
    QString real_icon_path = this->base_dir + "/" + icon_path;
    this->setWindowIcon(QIcon(real_icon_path));

}

QString opApp::get_output_tip() {
// "Runtime is PHP5.6.0|PHP7.1.6 \nMade by Freet Bash";
    QString res = "Runtime is ";
    for (QString a: this->phps.keys()){
        res += a + " | ";
    }
    res += "\nMade by Freet Bash";
    return res;

}

void opApp::run_code() {
    // generate temp php file
    QString temp_php = this->base_dir + "/temp.php";
    QFile f(temp_php);
    f.open(QIODevice::WriteOnly | QIODevice::Text);
    QString content = this->codeinput.toPlainText();
    QTextStream out(&f);
    out << content;
    f.close();
    cout << content.toStdString() << endl;

    QString php = this->phps[this->phpVersion.currentText()];
    QString result = execute_phpcode(php, temp_php);
    cout << result.toStdString() << endl;
    if (this->setHtml.isChecked()) {
        this->output.setHtml(result);
    } else {
        this->output.setPlainText(result);
    }
}

//获取目录下的文件夹名列表
QStringList GetDirNameList(const QString &strDirpath) {
    QDir dir(strDirpath);
    QFileInfoList Info_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    QListIterator<QFileInfo> Iterator(Info_list);
    QStringList strDirNameList;
    QFileInfo Info;
    while (Iterator.hasNext())
    {
        Info = Iterator.next();
        if (Info.isDir())
        {
            strDirNameList << QDir(Info.absoluteFilePath()).dirName();
        }
    }
    return strDirNameList;

}

QString execute_phpcode(QString php, QString temp_php) {
    cout << "enter execute" << endl;
    cout << php.toStdString() << endl;
    cout << temp_php.toStdString() << endl;

    QProcess process;
    process.setProgram(php);
    process.setArguments({"-f", temp_php});
    process.setProcessChannelMode(QProcess::SeparateChannels);

    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    process.setProcessEnvironment(environment);

    // Start the process
    process.start();

    // Wait for the process to finish
    process.waitForFinished();

    // Read standard output and standard error
    QString error = QString::fromStdString(process.readAllStandardError().toStdString());
    QString output = QString::fromStdString(process.readAllStandardOutput().toStdString());

    QString res = output;
    if (error.length() != 0 ) {
        res += "\nSTDERR: " + error;
    }

    return res;
}

