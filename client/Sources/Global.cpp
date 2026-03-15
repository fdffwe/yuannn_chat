#include"Global.hpp"

std::function<void(QWidget*)> repolish = [](QWidget* w){
    w->style()->unpolish(w);
    w->style()->polish(w);
};


QString GATE_URL_PREFIX = "";

