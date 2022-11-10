#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>

#include "createwavfile.h"
#include "createfilethread.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void createWavFile(CreateWavFile::SignalType st, int signalFreq, int maxGainIdx, int samplingFreq, int bitResolution, int nbOfChannels, float playTime);

private slots:
    void on_samplingFreqCB_activated(int index);
    void on_bitResolutionCB_activated(int index);
    void on_numOfChannelsCB_activated(int index);
    void on_filePlayTimeLE_textChanged(const QString &arg1);
    void on_generateFileBtn_clicked();
    void on_signalTypeCB_activated(int index);
    void on_signalFreqLE_textChanged(const QString &arg1);
    void onFileCreationProgress(int progress);
    void onFileCreated(bool result);

    void on_maxGainCB_activated(int index);

private:
    // functions
    QString getDefaultFileName(CreateWavFile::SignalType st, int signalFreq);

    // variables
    Ui::MainWindow *ui;
    CreateWavFile *createWavFileObj = Q_NULLPTR;
    CreateWavFile::SignalType signalType = CreateWavFile::SINE;
    CreateFileThread *createFileThread = Q_NULLPTR;

    int signalFreq = -1;
    int maxGainIdx = -1;
    int samplingFrequency = -1;
    int bitResolution = -1;
    int nbOfChannels = -1;
    float playTime = -1;
};
#endif // MAINWINDOW_H
