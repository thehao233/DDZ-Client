#include "roommenudialog.h"
#include "ui_roommenudialog.h"

#include "datamanager.h"

RoomMenuDialog::RoomMenuDialog(MenuType type, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RoomMenuDialog)
{
    ui->setupUi(this);
    setFixedSize(300, 180);

    // 设置菜单的描述信息
    const QString menuMsg[] = {"请输入要创建的房间名字：", "请输入要搜索的房间名字："};
    int index = static_cast<int>(type);
    ui->MenuMsgLable->setText(menuMsg[index]);
    // 设置显示的按钮组
    ui->stackedWidget->setCurrentIndex(index);

    connect(ui->createRoomBtn, &QPushButton::clicked, this, &RoomMenuDialog::sendCreateRoomReq);
    connect(ui->searchRoomBtn, &QPushButton::clicked, this, &RoomMenuDialog::sendSearchRoomReq);
    connect(ui->joinRoomBtn, &QPushButton::clicked, this, &RoomMenuDialog::sendJoinRoomReq);
}

RoomMenuDialog::~RoomMenuDialog()
{
    delete ui;
}

void RoomMenuDialog::sendCreateRoomReq()
{
    sendRequestMsg(RequestCode::CreateRoom);
}

void RoomMenuDialog::sendSearchRoomReq()
{
    sendRequestMsg(RequestCode::SearchRoom);
}

void RoomMenuDialog::sendJoinRoomReq()
{
    sendRequestMsg(RequestCode::JoinRoom);
}

void RoomMenuDialog::sendRequestMsg(RequestCode code)
{
    Message msg;
    msg.username = DataManager::getInstance()->getUserName();
    msg.roomname = ui->roomNameEdit->text().toUtf8();
    msg.reqCode = code;
    // 发送请求数据
    DataManager::getInstance()->getCommunication()->sendMsg(&msg);
}
