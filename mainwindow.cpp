#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // init combo box values
    ui->signalTypeCB->addItem(tr("Sine"), 0);

    ui->samplingFreqCB->addItem("8000", 8000);
    ui->samplingFreqCB->addItem("16000", 16000);
    ui->samplingFreqCB->addItem("32000", 32000);
    ui->samplingFreqCB->addItem("44100", 44100);
    ui->samplingFreqCB->addItem("46875", 46875);
    ui->samplingFreqCB->addItem("48000", 48000);
    ui->samplingFreqCB->addItem("96000", 96000);
    ui->samplingFreqCB->addItem("192000", 192000);

    ui->bitResolutionCB->addItem("8", 8);
    ui->bitResolutionCB->addItem("16", 16);
    ui->bitResolutionCB->addItem("24", 24);
    ui->bitResolutionCB->addItem("32", 32);

    ui->numOfChannelsCB->addItem("1", 1);
    ui->numOfChannelsCB->addItem("2", 2);

    ui->maxGainCB->addItem("1", 0);
    ui->maxGainCB->addItem("1/2", 1);
    ui->maxGainCB->addItem("1/4", 2);
    ui->maxGainCB->addItem("1/8", 3);
    ui->maxGainCB->addItem("1/16", 4);
    ui->maxGainCB->addItem("1/32", 5);
    ui->maxGainCB->addItem("1/64", 6);
    ui->maxGainCB->addItem("1/128", 7);

    // set validator for play time line edit
    QRegularExpression rxf("[-+]?[0-9]*.?[0-9]+");
    QRegularExpression rxd("^[0-9]{5}$");
    QValidator *validatorf = new QRegularExpressionValidator(rxf, this);
    QValidator *validatord = new QRegularExpressionValidator(rxd, this);
    ui->filePlayTimeLE->setValidator(validatorf);
    ui->signalFreqLE->setValidator(validatord);

    // init default values in combo boxes
    samplingFrequency = 8000;
    bitResolution = 8;
    nbOfChannels = 1;
    maxGainIdx = 0;
}

MainWindow::~MainWindow()
{
    this->disconnect();
    delete ui;
}

void MainWindow::on_signalTypeCB_activated(int index)
{
    signalType = ui->signalTypeCB->itemData(index).value<CreateWavFile::SignalType>();
}

void MainWindow::on_signalFreqLE_textChanged(const QString &arg1)
{
    bool isConverted;
    signalFreq = arg1.toInt(&isConverted);
    if(!isConverted)
    {
        signalFreq = -1;
    }
}

void MainWindow::on_samplingFreqCB_activated(int index)
{
    samplingFrequency = ui->samplingFreqCB->itemData(index).value<int>();
    if(signalFreq > samplingFrequency/2)
    {
        signalFreq = samplingFrequency/2;
        ui->signalFreqLE->setText(QString("%1").arg(signalFreq));
        QMessageBox::warning(this, tr("Error"), tr("Signal frequency more than half of sampling frequency. Limit signal frequency to 1/2 of sampling frequency"));
    }
}

void MainWindow::on_bitResolutionCB_activated(int index)
{
    bitResolution = ui->bitResolutionCB->itemData(index).value<int>();
}

void MainWindow::on_numOfChannelsCB_activated(int index)
{
    nbOfChannels = ui->numOfChannelsCB->itemData(index).value<int>();
}

void MainWindow::on_maxGainCB_activated(int index)
{
    maxGainIdx = ui->maxGainCB->itemData(index).value<int>();
}

void MainWindow::on_filePlayTimeLE_textChanged(const QString &arg1)
{
    bool isConverted;
    playTime = arg1.toFloat(&isConverted);
    if(!isConverted)
    {
        playTime = -1;
    }
}

void MainWindow::on_generateFileBtn_clicked()
{
    // check parameters
    if(signalFreq == -1)
    {
        QMessageBox::warning(this, tr("Error"), tr("Signal frequency is incorrect"));
        return;
    }
    if(maxGainIdx == -1)
    {
        QMessageBox::warning(this, tr("Error"), tr("Max gain is incorrect"));
        return;
    }
    if(samplingFrequency == -1)
    {
        QMessageBox::warning(this, tr("Error"), tr("Sampling frequency is incorrect"));
        return;
    }
    if(bitResolution == -1)
    {
        QMessageBox::warning(this, tr("Error"), tr("Bit resolution is incorrect"));
        return;
    }
    if(nbOfChannels == -1)
    {
        QMessageBox::warning(this, tr("Error"), tr("Number of channel is incorrect"));
        return;
    }
    if(playTime == -1)
    {
        QMessageBox::warning(this, tr("Error"), tr("File play time is incorrect"));
        return;
    }
    if(signalFreq > samplingFrequency/2)
    {
        signalFreq = samplingFrequency/2;
        ui->signalFreqLE->setText(QString("%1").arg(signalFreq));
        QMessageBox::warning(this, tr("Error"), tr("Signal frequency more than half of sampling frequency. Limit signal frequency to 1/2 of sampling frequency"));
        return;
    }
    // set out file location
    QString defaultFileName = getDefaultFileName(signalType, signalFreq);
    QString file_path = QFileDialog::getSaveFileName(this, tr("Save File"), defaultFileName, tr("Audio files (*.wav )"));
    if(file_path.isEmpty())
    {
        QMessageBox::warning(this, tr("Error"), tr("Output file name is incorrect"));
        return;
    }
    createWavFileObj = new CreateWavFile(file_path);
    createFileThread = new CreateFileThread();

    connect(this, &MainWindow::createWavFile, createWavFileObj, &CreateWavFile::onCreateWavFile);
    connect(createWavFileObj, &CreateWavFile::progressUpdate, this, &MainWindow::onFileCreationProgress);
    connect(createWavFileObj, &CreateWavFile::fileCreated, this, &MainWindow::onFileCreated);

    createWavFileObj->moveToThread(createFileThread);
    createFileThread->startThread();

    emit createWavFile(signalType, signalFreq, maxGainIdx, samplingFrequency, bitResolution, nbOfChannels, playTime);
}

void MainWindow::onFileCreationProgress(int progress)
{
    // check progress value limits
    if(progress < 0) progress = 0;
    if(progress > 100) progress = 100;

    ui->fileCreationPB->setValue(progress);
}

void MainWindow::onFileCreated(bool result)
{
    // stop firmware update thread
    createFileThread->stopThread();
    qDebug()<<"Finish create file task";
    delete createWavFileObj;

    if(result)
    {
        QMessageBox::warning(this, tr("Result"), tr("File was created successfully"));
    }
    else
    {
        QMessageBox::warning(this, tr("Error"), tr("File wasn't created"));
    }
}

QString MainWindow::getDefaultFileName(CreateWavFile::SignalType st, int signalFreq)
{
    QString file_name;
    switch(st)
    {
        case CreateWavFile::SINE:
        default:
            file_name = QString("sine_%1Hz_%2").arg(signalFreq).arg(ui->maxGainCB->currentText().replace("/", "_"));
            break;
    }
    return file_name;
}
