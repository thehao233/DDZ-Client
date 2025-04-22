#include "gamemode.h"
#include "gamepanel.h"
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

    connect(ui->netModeBtn, &QPushButton::clicked, this, [=]{
        // 切换到房间选择页面
        ui->stackedWidget->setCurrentWidget(ui->roomSelectPage);
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
