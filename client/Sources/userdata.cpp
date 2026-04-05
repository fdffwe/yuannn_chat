#include "userdata.h"
SearchInfo::SearchInfo(int uid, QString name,
    QString nick, QString desc, int sex, QString icon):_uid(uid)
  ,_name(name), _nick(nick),_desc(desc),_sex(sex),_icon(icon){
}

AddFriendApply::AddFriendApply(int from_uid, QString name, QString desc,
                               QString icon, QString nick, int sex)
    :_from_uid(from_uid),_name(name),
      _desc(desc),_icon(icon),_nick(nick),_sex(sex)
{

}

void FriendInfo::AppendChatMsgs(const std::vector<std::shared_ptr<TextChatData> > text_vec)
{
  for (const auto &text : text_vec) {
    // try to interpret msg id as number (server message_id). If not numeric, append at end.
    bool isNumber = false;
    long long mid = 0;
    bool ok = false;
    try {
      mid = text->_msg_id.toLongLong(&ok);
      isNumber = ok;
    } catch(...) { isNumber = false; }

    // deduplicate: if numeric id present and already exists, skip
    if (isNumber) {
      bool found = false;
      for (auto &existing : _chat_msgs) {
        long long emid = 0;
        bool eok = false;
        try { emid = existing->_msg_id.toLongLong(&eok); } catch(...) { eok = false; }
        if (eok && emid == mid) { found = true; break; }
      }
      if (found) continue;

      // insert in order by numeric message_id
      bool inserted = false;
      for (auto it = _chat_msgs.begin(); it != _chat_msgs.end(); ++it) {
        long long emid = 0; bool eok = false;
        try { emid = (*it)->_msg_id.toLongLong(&eok); } catch(...) { eok = false; }
        if (eok && emid > mid) {
          _chat_msgs.insert(it, text);
          inserted = true;
          break;
        }
      }
      if (!inserted) _chat_msgs.push_back(text);
    } else {
      // non-numeric id (local UUID) - check by exact id to avoid duplicates
      bool found = false;
      for (auto &existing : _chat_msgs) {
        if (existing->_msg_id == text->_msg_id) { found = true; break; }
      }
      if (!found) _chat_msgs.push_back(text);
    }
  }
}
