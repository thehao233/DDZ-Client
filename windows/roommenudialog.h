#ifndef ROOMMENUDIALOG_H
#define ROOMMENUDIALOG_H

#include <QDialog>
#include "codec.h"

namespace Ui {
class RoomMenuDialog;
}

class RoomMenuDialog : public QDialog
{
    Q_OBJECT

public:

    enum class MenuType:char{CreateRoom, SearchRoom};

    explicit RoomMenuDialog(MenuType type, QWidget *parent = nullptr);
    ~RoomMenuDialog();


    // 发送创建房间请求
    /** 服务器在处理这个请求时，会先创建房间，然后直接加入房间
        这与源码中 RequestCode为 ManualRoom 的处理类似 */
    void sendCreateRoomReq();
    // 发送搜索房间请求
    void sendSearchRoomReq();
    // 发送加入房间请求
    /** 将源码中 RequestCode为 ManualRoom的处理部分拆分到了这个函数*/
    void sendJoinRoomReq();

private:
    // 向服务器发送请求数据
    void sendRequestMsg(RequestCode code);

private:
    Ui::RoomMenuDialog *ui;
};

#endif // ROOMMENUDIALOG_H
