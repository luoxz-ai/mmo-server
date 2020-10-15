#ifndef PLAYERDIALOG_H_
#define PLAYERDIALOG_H_

#include "BaseCode.h"

export_lua enum DIALOG_FUNC_TYPE {
    DIALOG_FUNC_ACCEPTTASK      = 1,
    DIALOG_FUNC_SUBMITTASK      = 2,
    DIALOG_FUNC_QUICKFINISHTASK = 3,
    DIALOG_FUNC_SHOWTASK        = 4,
    DIALOG_FUNC_OPENSHOP        = 5,
    DIALOG_FUNC_SCRIPT          = 6,
};
export_lua enum DIALOG_TYPE {
    DIALOGTYPE_NORMAL          = 0,
    DIALOGTYPE_ACCEPT_TASK     = 1,
    DIALOGTYPE_WAITFINISH_TASK = 2,
    DIALOGTYPE_SUBMIT_TASK     = 3,
};

export_lua enum DIALOGLINK_TYPE {
    DIALOGLINK_TYPE_BUTTON = 0, //底部按钮
    DIALOGLINK_TYPE_LIST   = 1, //中间的列表式按钮
};

class SC_DIALOG;
class CPlayer;
export_lua class CPlayerDialog
{
protected:
    CPlayerDialog() {}
    bool Init(CPlayer* pPlayer);

public:
    CreateNewImpl(CPlayerDialog);

public:
    ~CPlayerDialog() {}

    export_lua bool DialogBegin(const std::string& title);
    export_lua bool DialogAddText(const std::string& txt);
    export_lua bool DialogAddLink(uint32_t           nLinkType,
                                  const std::string& link_txt,
                                  uint32_t           idFuncType,
                                  uint64_t           idData,
                                  const std::string& callback_func,
                                  uint64_t           idNpc = 0);
    export_lua bool DialogSend(uint32_t nDialogType = 0);
    bool            OnDialogClick(uint64_t idDialog, uint32_t nIdx);

    export_lua bool ActiveNpc(OBJID idNpc);

private:
    CPlayer* m_pOwner;

    struct ST_CALLBACK_INFO
    {
        uint32_t    idFuncType;
        uint64_t    idData;
        std::string callback_func;
        uint64_t    idNpc;
    };
    std::map<uint64_t, std::vector<ST_CALLBACK_INFO>> m_dialog_callback;

    std::unique_ptr<SC_DIALOG> m_dialog_msg;
};
#endif // PLAYERDIALOG_H_