#include "gamemode.h"
#include "gamepanel.h"
#include "roommenudialog.h"
#include "ui_gamemode.h"

GameMode::GameMode(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::GameMode)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentWidget(ui->modeSelectPage);

    connect(ui->singleModeBtn, &QPushButton::clicked, this, [=]{
        // 进入单机游戏界面
        GamePanel* panel = new GamePanel;

        connect(panel, &GamePanel::closeGamePanel, this, [=]{
            ui->stackedWidget->setCurrentWidget(ui->modeSelectPage);
            show();
        });
        panel->show();

        // 隐藏 当前gamemode界面
        hide();
    });

    // 房间相关操作
    connect(ui->netModeBtn, &QPushButton::clicked, this, [=]{
        // 切换到房间选择页面
        ui->stackedWidget->setCurrentWidget(ui->roomSelectPage);
    });

    // 随机加入某个房间
    connect(ui->autoRoomBtn, &QPushButton::clicked, this, [=]{
        emit autoJoinRoom();
        qDebug() << "开始随机加入一个房间.....";
    });
    // 手动加入某个房间
    connect(ui->createRoomBtn, &QPushButton::clicked, this, [=]{
        RoomMenuDialog room(RoomMenuDialog::MenuType::CreateRoom);
        room.setWindowTitle("创建房间");
        room.exec();
    });
    // 搜索某个房间
    connect(ui->searchRoomBtn, &QPushButton::clicked, this, [=](){
        RoomMenuDialog room(RoomMenuDialog::MenuType::SearchRoom);
        room.setWindowTitle("搜索房间");
        room.exec();
    });

}

GameMode::~GameMode()
{
    delete ui;
}

void GameMode::closeEvent(QCloseEvent *ev)
{
    if (ui->stackedWidget->currentWidget() == ui->roomSelectPage)
    {
        // 返回 模式选择界面
        ui->stackedWidget->setCurrentWidget(ui->modeSelectPage);
        ev->ignore();
    }
    else
    {
        // 关闭gamemode界面
        ev->accept();
        // deleteLater();
    }
}
