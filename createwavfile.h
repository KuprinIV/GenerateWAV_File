#ifndef CREATEWAVFILE_H
#define CREATEWAVFILE_H

#include <QObject>
#include <QDebug>
#include <QSaveFile>

class CreateWavFile : public QObject
{
    Q_OBJECT
public:
    CreateWavFile(QString path);
    ~CreateWavFile();

    enum SignalType
    {
        SINE = 0,
    };

public slots:
    void onCreateWavFile(CreateWavFile::SignalType st, int signalFreq, int maxGainIdx, int samplingFreq, int bitResolution, int nbOfChannels, float playTime);

signals:
    void progressUpdate(int progress);
    void fileCreated(bool result);

private:
    // functions
    void setFileHeader(QByteArray* data, int samplingFreq, int bitResolution, int nbOfChannels, float playTime);
    void generateSineWave(QByteArray* data, int signalFreq, int maxGainIdx, int samplingFreq, int bitResolution, int nbOfChannels);
    bool saveFile(QByteArray* data, QString file_path);

    // variables
    QString outFilePath;
    QByteArray wavFileData;
    int headerOffset = 44;
    int fileSize = 0;
    int currentProgress = 0;
    int prevProgress = 0;

};

#endif // CREATEWAVFILE_H
