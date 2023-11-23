#include "widget.h"
#include "ui_widget.h"
#include <QStyle>
#include <QFileDialog>
#include <QDir>
#include <QTime>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    muted = false;

    //                             Buttons style:
    ui->pushButtonOpen->setIcon(style()->standardIcon(QStyle::SP_DriveCDIcon));
    ui->pushButtonPrev->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    ui->pushButtonPlay->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    ui->pushButtonPause->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    ui->pushButtonStop->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
    ui->pushButtonNext->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    ui->pushButtonMute->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    ui->pushButtonRepeat->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
    ui->pushButtonDEL->setIcon(style()->standardIcon(QStyle::SP_LineEditClearButton));
    ui->pushButtonCLR->setIcon(style()->standardIcon(QStyle::SP_BrowserStop));

    //                             Player init:
    m_player = new QMediaPlayer(this);
    m_player->setVolume(70);//громкость воспроизведения
    ui->labelVolume->setText(QString("Volume: ").append(QString::number(m_player->volume())));
    ui->horizontalSliderVolume->setValue(m_player->volume());

    connect(m_player,&QMediaPlayer::positionChanged, this,&Widget::on_position_changed);
    connect(m_player,&QMediaPlayer::durationChanged, this,&Widget::on_duration_changed);

    //                             Playlist init:
    m_playlist_model = new QStandardItemModel(this);
    ui->tableViewPlaylist->setModel(m_playlist_model);
    m_playlist_model->setHorizontalHeaderLabels(QStringList() << "Audio track" << "File path");
    ui->tableViewPlaylist->hideColumn(1);
    ui->tableViewPlaylist->horizontalHeader()->setStretchLastSection(true);
    ui->tableViewPlaylist->setEditTriggers(QAbstractItemView::NoEditTriggers);


    //Инициализация плейлиста
    m_playlist = new QMediaPlaylist(m_player);
    m_player->setPlaylist(m_playlist);
    //Двойное нажатие воспроизвеледние
    connect(ui->tableViewPlaylist, &QTableView::doubleClicked,[this](const QModelIndex &index)
    {
        m_playlist->setCurrentIndex(index.row());
        m_player->play();
    });
    //Выделение трека
}

Widget::~Widget()
{
    delete m_player;
    delete  m_playlist;
    delete  m_playlist_model;
    delete ui;
}


void Widget::on_pushButtonOpen_clicked()
{
//    QString file = QFileDialog::getOpenFileName
//            (
//                this, //Родительское окно
//                tr("Open file"), // Заголовок окна диалога
//                "C:\\Музыка\\Музыка", //Рабочий каталог
//                tr("Audio files (*.mp3 *.flac)")
//                );
//    ui->labelComposition->setText(file.split('/').last());
//    this->setWindowTitle(QString("Winamp - ").append(file.split('/').last()));
//    m_player->setMedia(QUrl::fromLocalFile(file));

    QStringList files = QFileDialog::getOpenFileNames
            (
                this,
                "Open files",
                "C:\\Музыка\\Музыка",
                "Audio files (*.mp3 *.flac)"
                );
    for(QString filesPath: files)
    {
        QList<QStandardItem*> items;
        items.append(new QStandardItem(QDir(filesPath).dirName()));
        items.append(new QStandardItem(filesPath));
        m_playlist_model->appendRow(items);
        m_playlist->addMedia(QUrl(filesPath));
    }
}


void Widget::on_horizontalSliderVolume_valueChanged(int value)
{
    m_player->setVolume(value);
    ui->labelVolume->setText(QString("Vilume:").append(QString::number(m_player->volume())));
}


void Widget::on_pushButtonPlay_clicked()
{
    m_player->play();
}

void Widget::on_position_changed(qint64 position)
{
   ui->horizontalSliderProgress->setValue(position);
   QTime qt_position = QTime::fromMSecsSinceStartOfDay(position);
   ui->labelProgress->setText(QString("Position: ").append(qt_position.toString("mm:ss")));
}
void Widget::on_duration_changed(qint64 duration)
{
    ui->horizontalSliderProgress->setMaximum(duration);
    QTime qt_duration = QTime::fromMSecsSinceStartOfDay(duration);//Всемя пройденное с момента начала
    ui->labelDuration->setText(QString("Duration: ").append(qt_duration.toString("mm:ss")));
}
void Widget::on_horizontalSliderProgress_sliderMoved(int position)
{
    m_player->setPosition(position);
}
void Widget::on_pushButtonMute_clicked()
{
   muted = !muted;
   m_player->setMuted(muted);
   ui->pushButtonMute->setIcon(style()->standardIcon(muted?QStyle::SP_MediaVolumeMuted:QStyle::SP_MediaVolume));
}


void Widget::on_pushButtonPrev_clicked()
{
    m_playlist->previous();
}


void Widget::on_pushButtonNext_clicked()
{
    m_playlist->next();
}


void Widget::on_pushButtonPause_clicked()
{
    m_player->pause();
}


void Widget::on_pushButtonStop_clicked()
{
    m_player->stop();
}


void Widget::on_pushButtonRepeat_clicked()
{
    m_playlist->setPlaybackMode(QMediaPlaylist::Loop);//зацикливает плейлист
}


void Widget::on_pushButtonRandom_clicked()
{
     m_playlist->setPlaybackMode(QMediaPlaylist::Random);//Воспроизведение в случайном порядке
}


void Widget::on_pushButtonCLR_clicked()
{
    m_playlist->clear();
    m_playlist_model->clear();
}


void Widget::on_pushButtonDEL_clicked()
{

}

