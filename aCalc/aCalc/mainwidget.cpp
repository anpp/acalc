#include "mainwidget.h"
#include "buttexts.h"
#include "dialogsettings.h"
#include "dialogvariables.h"
#include "functors.h"

inline int GetHFButton(int h)
{
    return h - DIFF_HEIGHT > MIN_HEIGHT? h - DIFF_HEIGHT: MIN_HEIGHT;
}


//----------------------------------------------------------------------------------------------------------------------
MainWidget::MainWidget(QWidget *parent) :
        QWidget(parent, Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint),
    loger(log_dirname, log_filename)
{

    settings.loadSettings();

    this->setContentsMargins(spacing / 2, 0, spacing / 2, spacing / 2);

    setWindowIcon(QIcon(":/Icons/calculator.ico"));

    QApplication::setStyle("fusion");
    //QApplication::setStyle("freeze");

    CreateMenus();    
    CreateWidgets();    
    InitLocale();
    DefaultKeysMap();

    parser->SetVariable("default_color", "#000000", e_type_var::STRING);
    parser->SetVariable("number_color", String(pb.getColor(Pnl::Dig)->name().toStdString().c_str()), e_type_var::STRING);
    parser->SetVariable("function_color", String(pb.getColor(Pnl::Func)->name().toStdString().c_str()), e_type_var::STRING);
    parser->SetVariable("delimiter_color", String(pb.getColor(Pnl::Op)->name().toStdString().c_str()), e_type_var::STRING);

    SendKey(Qt::Key_F3); //Rad
    SendKey(Qt::Key_F6); //Dec

    EnableLogList();

    LoadMemory();
    LoadExpression();

    this->setFocus();    
}


//----------------------------------------------------------------------------------------------------------------------
MainWidget::~MainWidget()
{
    SaveExpression(parser->GetExpression() + (wDisplay->result().isEmpty()? "": " ="));
    settings.saveSettings();

    delete cbxlogList;
    delete parser;
    delete mePress;
    delete meRelease;
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::ReCreateMouseEvents()
{
    delete mePress;
    delete meRelease;

    mePress = new QMouseEvent(QEvent::MouseButtonPress, posMousePress, Qt::LeftButton,
                                           Qt::LeftButton, Qt::NoModifier);
    meRelease = new QMouseEvent(QEvent::MouseButtonRelease, posMousePress, Qt::LeftButton,
                                Qt::LeftButton, Qt::NoModifier);
}


//----------------------------------------------------------------------------------------------------------------------
bool MainWidget::isLoging()
{
    return (settings.getSetting("Loging").toBool());
}


//----------------------------------------------------------------------------------------------------------------------
bool MainWidget::isLogList()
{
    return (settings.getSetting("logList").toBool() && cbxlogList != nullptr);
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::EnableLogList()
{
    if(cbxlogList != nullptr)
        cbxlogList->setVisible(settings.getSetting("logList").toBool());
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::LoadMemory()
{
    SetMemory(settings.getSetting("memory").toString());
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::SaveMemory(const QString& value)
{
   settings.setSetting("memory", value);
   settings.saveSettingsByKind(kindset::state);
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::CreateMenus(void)
{
    MenuBar = new QMenuBar(this);
    MenuBar->setContentsMargins(0, 0, 0, 0);

    MenuView = new QMenu();
    MenuEdit = new QMenu();
    MenuHelp = new QMenu();
    MenuLanguages = new QMenu();

    MenuBar->addMenu(MenuView);
    MenuBar->addMenu(MenuEdit);
    MenuBar->addMenu(MenuHelp);

    for (auto& s: sViews)
    {
        ActionViews.append(MenuView->addAction(s));
        ActionViews.last()->setCheckable(true);
    }
    ActionViews.last()->setEnabled(false);
    connect(MenuView, SIGNAL(triggered(QAction*)), SLOT(slotView(QAction*)));

    for (auto& s: sLanguages)
    {
        ActionLanguages.append(MenuLanguages->addAction(s));
        ActionLanguages.last()->setCheckable(true);
    }
    connect(MenuLanguages, SIGNAL(triggered(QAction*)), SLOT(slotLanguage(QAction*)));


    ActionCopy = MenuEdit->addAction("", this, SLOT(slotCopy(void)), Qt::CTRL + Qt::Key_Insert);
    ActionPaste = MenuEdit->addAction("", this, SLOT(slotPaste(void)), Qt::SHIFT + Qt::Key_Insert);
    MenuEdit->addSeparator();
    MenuEdit->addMenu(MenuLanguages);

    ActionVariables = MenuEdit->addAction("", this, SLOT(slotVariables(void)));
    ActionSettings = MenuEdit->addAction("", this, SLOT(slotSettings(void)));    

    ActionContextHelp = MenuHelp->addAction("", this, SLOT(slotEnterWhatIsThis(void)), Qt::SHIFT + Qt::Key_F1);
    ActionAbout = MenuHelp->addAction("", this, SLOT(slotAbout(void)));

    MenuOnButton = new QMenu(this);
    ActionWhatIs = MenuOnButton->addAction("", this, SLOT(slotMenuWhatIsThis(void)));
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::SetLocaleTexts()
{
    setWindowTitle(tr("Calculator"));

    MenuView->setTitle(tr("View"));
    MenuEdit->setTitle(tr("Edit"));
    MenuHelp->setTitle(tr("Help"));
    MenuLanguages->setTitle(tr("Languages"));
    ActionCopy->setText(tr("&Copy"));
    ActionPaste->setText(tr("&Paste"));
    ActionSettings->setText(tr("Settings"));
    ActionVariables->setText(tr("Variables"));
    ActionContextHelp->setText(tr("Contex help"));
    ActionWhatIs->setText(tr("What is this?"));
    ActionAbout->setText(tr("About..."));


    for (QAction *action: ActionViews) {
        action->setText(QObject::tr(sViews[ActionViews.indexOf(action)].toStdString().c_str()));
    }
    for (QAction *action: ActionLanguages) {
        action->setText(tr(sLanguages[ActionLanguages.indexOf(action)].toStdString().c_str()));
    }
    for (QCalcWidget *w: vec_btns) {w->LoadWhatIsText();}
}

//----------------------------------------------------------------------------------------------------------------------
void MainWidget::slotView(QAction* action)
{
    if(action == nullptr) return;
    settings.setSetting("appview", ActionViews.indexOf(action));
    SetView(ActionViews.indexOf(action));
}

//----------------------------------------------------------------------------------------------------------------------
void MainWidget::slotLanguage(QAction* action)
{
    if(action == nullptr) return;
    settings.setSetting("Language", ActionLanguages.indexOf(action));
    SetLocale(static_cast<Langs>(ActionLanguages.indexOf(action)));
}

void MainWidget::slotOnPopupLogList()
{
    std::unique_ptr<QStringList> logs;
    bool to_begin = false;

    if(logModel.rowCount(QModelIndex()) == 0)
        logs = loger.ReadLast(settings.getSetting("log_rate").toInt());
    else
    {
        logs = loger.ReadLast();
        to_begin = true;
    }

    logModel.addItems(*logs.get(), to_begin);
}

void MainWidget::slotActivatedLogList(const QString &value)
{
    if(!value.isEmpty())
    {
        auto log_value = value;
        if(log_value.contains('='))
            log_value = log_value.left(log_value.indexOf('=') - 1);
        pasteExpression(log_value);
    }
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::slotAbout(void)
{
    QMessageBox::about(this, tr("About"), tr("AboutProgram"));
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::slotCopy(void)
{
  QString s = parser->GetExpression();
  if(!wDisplay->result().isEmpty())
  {
      if(parser->Run())
          s = s + " = " + parser->DoubleToString(parser->GetResult(), PRECISION_FOR_DOUBLE);
      else
          s = s + " " + parser->listErrors();
  }
  QApplication::clipboard()->setText(s);
}

//----------------------------------------------------------------------------------------------------------------------
void MainWidget::slotPaste(void)
{    
    QString pasteString = QApplication::clipboard()->text();
    if(pasteString.isEmpty() || "" == pasteString)
        return;

    pasteExpression(pasteString);
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::slotSettings(void)
{
  QPointer<DialogSettings> dialog_settings = new DialogSettings(&settings, this);
  dialog_settings->setAttribute(Qt::WA_DeleteOnClose);

  if (dialog_settings->exec() == QDialog::Accepted)
  {      
      EnableLogList();

      if(settings.getSetting("appview").toInt() != 2) //programming
        SetView(settings.getSetting("appview").toInt());
      ResizeAll(settings.getSetting("button_width").toUInt(), settings.getSetting("button_height").toUInt());
      SetLocale(static_cast<Langs>(settings.getSetting("Language").toInt()));

      settings.saveSettingsByKind(kindset::appearance);
      settings.saveSettingsByKind(kindset::misc);

      if(settings.isChanged("log_rate"))
        logModel.clear();
  }
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::slotVariables(void)
{
  DialogVariables* dialog_variables = new DialogVariables(this);
  if (dialog_variables->exec() == QDialog::Accepted)
  {
  }
  delete dialog_variables;
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::pasteExpression(const QString &exp)
{
    QString value, num_val;
    QString expression = exp;

    SendKey(Qt::Key_Escape);

    TokenList Tokens;
    Token tok;
    CalcParser tempParser;
    sKeyMod km = {0, 0};
    QCalcWidget *w = nullptr;

    tempParser.SetParams(&expression, 0, parser->DRG());
    Tokens = tempParser.RefTokens();

    bPasting = true;
    for(auto it = Tokens.begin(); it != Tokens.end(); ++it)
    {
        tok = *it;
        value = tok.Value();
        if(tok.Type() == t_type::UNK || tok.Type() == t_type::ERR) continue;
        if(tok.Type() == t_type::NUMBER)
        {
            for(String::iterator sit = value.begin(); sit != value.end(); ++sit)
            {
                num_val = *sit;
                if(num_val == ",")
                    num_val = ".";

                //Следующие замены для корректной вставки чисел в формате 1.e+01
                if(num_val == "e")
                    num_val = "exp";

                if(num_val == "+")
                    num_val = "";

                if(num_val == "-")
                    num_val = "+/-";
                km = FindKeyByValue(num_val);
                SendKey(km.key, km.mod);
            }

        }
        else
        {
            SetMode(SModes::Hyp, false);
            SetMode(SModes::Inv, false);
            km = FindKeyByValue(value);
            if(km.key == 0 && km.mod == 0) //Если не найдено сочетание клавиш для функции
                continue;

            w = FindWidgetByKey(km);
            if(w)
            {
                if(tok.Type() == t_type::FUNCTION && tok.Value() != w->Value())
                {
                    if(w->Hypable())
                        SetMode(SModes::Hyp, true);
                    else
                        if(w->Invable())
                            SetMode(SModes::Inv, true);
                }
                ClickToWidget(w->widget);
            }

            if(tok.Value() == "=")
                break;
        }
    }
    bPasting = false;
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::SendKey(int Key, unsigned int Mod)
{
    QKeyEvent *ke = new QKeyEvent(QEvent::KeyPress, Key, static_cast<Qt::KeyboardModifiers>(Mod));
    QApplication::sendEvent(this, ke);
    delete ke;
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::ClickToWidget(QWidget *widget, int msec)
{
    QElapsedTimer timer;
    if(widget)
    {
        if(!widget->isVisible())
            (static_cast<QAbstractButton*>(widget))->click();
        else
        {
            if(!(mePress && meRelease)) return;
            QApplication::sendEvent(widget, mePress);
            timer.start();
            while (!timer.hasExpired(msec)) {}
            QApplication::sendEvent(widget, meRelease);
        }
    }
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::focusOutEvent(QFocusEvent * fe)
{
    this->setFocus();
    QWidget::focusOutEvent(fe);
}


//----------------------------------------------------------------------------------------------------------------------
sKeyMod MainWidget::FindKeyByValue(QString value)
{
    sKeyMod km = {0, 0};
    auto mit = map_val_key.find(value);
    if(mit != map_val_key.end())
        km = *mit;
    return km;
}


//----------------------------------------------------------------------------------------------------------------------
QCalcWidget* MainWidget::FindWidgetByKey(sKeyMod km)
{
    auto mit = map_keys.find(km);
    if(mit != map_keys.end())
        return *mit;
    return nullptr;
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::keyPressEvent(QKeyEvent* pe)
{
    QCalcWidget *cw;
    sKeyMod km{pe->key(), pe->modifiers()};

    cw = FindWidgetByKey(km);
    if(!cw && km.mod)
    {
        km.mod = 0;
        cw = FindWidgetByKey(km);

    }
    if(cw)
        ClickToWidget(cw->widget);
    else
        QWidget::keyPressEvent(pe);
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::mousePressEvent(QMouseEvent *mpe)
{
    mpe->accept();
    curr_widget = nullptr;

    curr_widget = this->childAt(mpe->pos());

    curr_GlobalPos = mpe->globalPos();
    if(mpe->button() == Qt::RightButton && !QWhatsThis::inWhatsThisMode())
        if(curr_widget)
            MenuOnButton->exec(mpe->globalPos());

}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::slotMenuWhatIsThis(void)
{
    if(curr_widget)
        QWhatsThis::showText(curr_GlobalPos, curr_widget->whatsThis());
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::slotEnterWhatIsThis(void)
{
    QWhatsThis::enterWhatsThisMode();
}


//----------------------------------------------------------------------------------------------------------------------
bool MainWidget::event(QEvent *e)
{
    if((e->type() == QEvent::Leave || e->type() == QEvent::LeaveWhatsThisMode) && curr_widget)
        curr_widget->update();

    return QWidget::event(e);
}

void MainWidget::showEvent(QShowEvent *event)
{
    SetView(settings.getSetting("appview").toInt(), true);
    this->QWidget::showEvent(event);
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::InitLocale()
{
    qtTrans = new QTranslator(this);
    qtTransPopup = new QTranslator(this);
    qtTransErrors = new QTranslator(this);

    Langs lang = Langs::En;
    QLocale locale;

    if(settings.getSetting("Language").toInt() == 0)
    {
        if(locale.language() == QLocale::Russian)
            lang = Langs::Ru;
        if(locale.language() == QLocale::English)
            lang = Langs::En;
    }
    else
        lang = static_cast<Langs>(settings.getSetting("Language").toInt());

    SetLocale(lang);
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::SetLocale(Langs indexLang)
{
    int selectedIndex = static_cast<int>(indexLang);
    int index = 0;

    LoadLocale(sShortLanguages[selectedIndex]);
    for (QAction *action: ActionLanguages){
        action->setChecked(selectedIndex == index++);
    }
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::SetView(int indexView, bool isFirst)
{
    int index = 0;
    for (QAction *action: ActionViews){
        action->setChecked(indexView == index++);
    }

    if(settings.isChanged("appview") || isFirst)
    {
        if(!InitLayouts())
            QApplication::exit(-1);
    }
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::LoadLocale(const QString& sloc)
{
    QString localePath = ":/locales";

    qtTrans->load("acalc_" + sloc, localePath);
    qtTransPopup->load("popup_" + sloc, localePath);
    qtTransErrors->load("errors_" + sloc, localePath);

    qApp->installTranslator(qtTrans);
    qApp->installTranslator(qtTransPopup);
    qApp->installTranslator(qtTransErrors);
    SetLocaleTexts();
}


//----------------------------------------------------------------------------------------------------------------------
bool MainWidget::InitLayouts()
{
    QCalcWidget* rw;
    QString rv;

    FreeLayouts();

    switch(static_cast<CalcView>(settings.getSetting("appview").toInt()))
    {
    case CalcView::Original:
        for (QCalcWidget* widget: vec_btns) {
            if(widget->GetType() == Pnl::Nop) widget->SetType(widget->GetSavedType());
            if(widget->IsReplace()) widget->UnReplace();
        }
        LayoutOriginal();
        break;
    case CalcView::Simple:
        for (QCalcWidget* widget: vec_btns) {
            rv = widget->GetReplaceValue();
            if("" != rv)
            {
                rw = FindButtonByValue(rv);
                if(rw) rw->Replace(widget);
            }

            if(widget->IsNopable()) widget->SetType(Pnl::Nop);
        }
        LayoutSimple();
        break;
    case CalcView::Programmable:
        LayoutProgrammable();
        break;
    }

    for(auto i = 0; i < static_cast<int>(Pnl::Nop); ++i)
    {
        Pnl atype = static_cast<Pnl>(i);
            FillLayoutWidgets(atype);
    }

    ResizeAll(settings.getSetting("button_width").toUInt(), settings.getSetting("button_height").toUInt());
    return true;
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::FreeLayouts(void)
{
    CalcView appview = static_cast<CalcView>(settings.getSetting("appview").toInt());
    pb.getPanel(Pnl::Abc)->setVisible(appview == CalcView::Original);
    pb.getPanel(Pnl::Mem)->setVisible(appview == CalcView::Original);
    pb.getPanel(Pnl::Func)->setVisible(appview == CalcView::Original);
    pb.getPanel(Pnl::Scale)->setVisible(appview == CalcView::Original);
    pb.getPanel(Pnl::Drg)->setVisible(appview == CalcView::Original);
    pb.getPanel(Pnl::FuncModes)->setVisible(appview == CalcView::Original);


    for(std::pair<Pnl, QFrame*> panel: pb.panels())
    {
        QLayout *l = panel.second->layout();
        for(int i = 0; i < l->count(); ++i)
            l->removeItem(l->itemAt(i));
    }

    this->repaint();
    delete wBottom->layout();
    delete wMode->layout();
    wDisplay->deleteLayout();
    delete this->layout();
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::FillLayoutWidgets(Pnl atype)
{
    std::for_each(vec_btns.begin(), vec_btns.end(), [=] (QCalcWidget* w)
    {
        if(w->GetType() == atype)
            static_cast<QGridLayout*>(pb.getPanel(atype)->layout())->addWidget(w->widget, static_cast<int>(w->i), static_cast<int>(w->j), static_cast<int>(w->n_rows), static_cast<int>(w->n_cols));
    });
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::LayoutOriginal(void)
{
    QVBoxLayout *lMain = new QVBoxLayout();
    QVBoxLayout *lMenu = new QVBoxLayout();
    QHBoxLayout *lBottom = new QHBoxLayout();
    QBoxLayout *lMemAbc = new QVBoxLayout();    

    lMain->setContentsMargins(0, 0, 0, 0);
    lMain->setSpacing(spacing);

    lMenu->setContentsMargins(0, 0, 0, 0);
    lMenu->setSpacing(0);
    lMenu->addWidget(MenuBar);


    lMain->addLayout(lMenu);
    lMain->addWidget(cbxlogList);
    lMain->addWidget(wDisplay);
    lMain->addWidget(wMode);
    lMain->addWidget(pb.getPanel(Pnl::Func));
    lMain->addWidget(wBottom);


    lMemAbc->setMargin(0);
    lMemAbc->setSpacing(spacing);
    lMemAbc->addWidget(pb.getPanel(Pnl::Mem));
    lMemAbc->addWidget(pb.getPanel(Pnl::Abc));

    lBottom->setMargin(0);
    lBottom->setSpacing(spacing);
    lBottom->addWidget(pb.getPanel(Pnl::Dig), Qt::AlignLeft);
    lBottom->addWidget(pb.getPanel(Pnl::Op), Qt::AlignLeft);
    lBottom->addLayout(lMemAbc);

    InitModesLayouts();


    wDisplay->Layout();
    wBottom->setLayout(lBottom);

    this->setLayout(lMain);
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::LayoutSimple(void)
{
    QVBoxLayout *lMain = new QVBoxLayout();
    QVBoxLayout *lMenu = new QVBoxLayout();
    QHBoxLayout *lBottom = new QHBoxLayout();
    QHBoxLayout *lMode = new QHBoxLayout();

    //lMain->setContentsMargins(spacing, 0, spacing, spacing);
    lMain->setContentsMargins(0, 0, 0, 0);
    lMain->setSpacing(spacing);

    lMenu->setContentsMargins(0, 0, 0, 0);
    lMenu->setSpacing(0);
    lMenu->addWidget(MenuBar);

    lMain->addLayout(lMenu);
    lMain->addWidget(cbxlogList);
    lMain->addWidget(wDisplay);
    lMain->addWidget(wMode);
    lMain->addWidget(wBottom);

    lBottom->setMargin(0);
    lBottom->setSpacing(spacing);
    lBottom->addWidget(pb.getPanel(Pnl::Dig), Qt::AlignLeft);
    lBottom->addWidget(pb.getPanel(Pnl::Op), Qt::AlignLeft);

    lMode->setContentsMargins(0, 0, spacing, 0);
    lMode->setSpacing(spacing);
    lMode->addStretch();
    lMode->addWidget(pb.getPanel(Pnl::ServButtons));

    wBottom->setLayout(lBottom);
    wMode->setLayout(lMode);
    wDisplay->Layout();
    this->setLayout(lMain);
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::LayoutProgrammable(void)
{
//
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::InitModesLayouts()
{
    QHBoxLayout *lModeBottom = new QHBoxLayout();
    QVBoxLayout *lMode = new QVBoxLayout();
    QHBoxLayout *lModeTop = new QHBoxLayout();

    lMode->setMargin(0);
    lMode->setSpacing(spacing);
    lMode->addLayout(lModeTop);
    lMode->addLayout(lModeBottom);

    lModeBottom->setMargin(0);
    lModeBottom->setSpacing(spacing);

    lModeBottom->addWidget(pb.getPanel(Pnl::Scale));
    lModeBottom->addWidget(pb.getPanel(Pnl::Drg));

    lModeTop->setContentsMargins(0, 0, spacing, 0);
    lModeTop->setSpacing(spacing);
    lModeTop->addWidget(pb.getPanel(Pnl::FuncModes));
    lModeTop->addStretch();
    lModeTop->addWidget(pb.getPanel(Pnl::ServButtons));

    wMode->setLayout(lMode);
}



//----------------------------------------------------------------------------------------------------------------------
void MainWidget::CreateWidgets()
{
    wDisplay = new CalcDisplay(this);
    wMode = new QFrame(this);
    wBottom = new QFrame(this);

    cbxlogList = new QLogComboBox(this);
    cbxlogList->setModel(&logModel);
    connect(cbxlogList, SIGNAL(OnPopup()), SLOT(slotOnPopupLogList()));
    connect(cbxlogList, SIGNAL(activated(const QString&)), SLOT(slotActivatedLogList(const QString&)));


    wBottom->setContentsMargins(0, 0, 0, 0);
    wMode->setContentsMargins(0, 0, 0, 0);

    wDisplay->setContentsMargins(0, 0, 0, 0);

    wDisplay->setFrameStyle(shape);

    CreateButtons(Pnl::Dig);
    CreateButtons(Pnl::Op);
    CreateButtons(Pnl::Mem);
    CreateButtons(Pnl::Abc);
    CreateButtons(Pnl::Func);
    CreateButtons(Pnl::Scale);
    CreateButtons(Pnl::Drg);
    CreateButtons(Pnl::FuncModes);
    CreateButtons(Pnl::ServButtons);
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::CreateButtons(Pnl atype)
{
    QString *s;
    QString *s_v;
    int index;

    QCalcWidget *cw;
    auto max_i = pb.getRows(atype);
    auto max_j = pb.getCols(atype);

    QColor *color = pb.getColor(atype);

    switch(atype)
    {
    case Pnl::Dig:
    {
        s = sDigits;
        s_v = svDigits;
        break;
    }
    case Pnl::Op:
    {
        s = sOperators;
        s_v = svOperators;
        break;
    }
    case Pnl::Abc:
    {
        s = sAbc;
        s_v = sAbc;
        break;
    }
    case Pnl::Mem:
    {
        s = sMem;
        s_v = sMem;

        lblMem = new QLabel();
        lblMem->setFrameStyle(QFrame::WinPanel);
        lblMem->setAlignment(Qt::AlignCenter);
        vec_btns.push_back(new QCalcWidget(lblMem, 1, 0, atype, "", "", 1, 3, true));
        break;
    }
    case Pnl::Func:
    {
        s = sFunc;
        s_v = svFunc;
        break;
    }
    case Pnl::Scale:
        vec_btns.push_back(new QCalcWidget(new QRadioButton(sScales[static_cast<int>(Scales::Hex)]), 0, 0, atype));
        vec_btns.push_back(new QCalcWidget(new QRadioButton(sScales[static_cast<int>(Scales::Dec)]), 0, 1, atype));
        vec_btns.push_back(new QCalcWidget(new QRadioButton(sScales[static_cast<int>(Scales::Oct)]), 0, 2, atype));
        vec_btns.push_back(new QCalcWidget(new QRadioButton(sScales[static_cast<int>(Scales::Bin)]), 0, 3, atype));
        for (auto it: vec_btns)
        {
            cw = it;
            if(cw->GetType() == atype)
                connect(cw, SIGNAL(ClickServButton(QString)), SLOT(ProcessClickScale(QString)));
        }
        return;
    case Pnl::Drg:
        vec_btns.push_back(new QCalcWidget(new QRadioButton(sDrg[static_cast<int>(Drg::Deg)]), 0, 0, atype));
        connect(vec_btns.back(), SIGNAL(ClickServButton(QString)), SLOT(ProcessClickDRG(QString)));

        vec_btns.push_back(new QCalcWidget(new QRadioButton(sDrg[static_cast<int>(Drg::Rad)]), 0, 1, atype));
        connect(vec_btns.back(), SIGNAL(ClickServButton(QString)), SLOT(ProcessClickDRG(QString)));

        vec_btns.push_back(new QCalcWidget(new QRadioButton(sDrg[static_cast<int>(Drg::Grad)]), 0, 2, atype));
        connect(vec_btns.back(), SIGNAL(ClickServButton(QString)), SLOT(ProcessClickDRG(QString)));
        return;
    case Pnl::FuncModes:
        vec_btns.push_back(new QCalcWidget(new QCheckBox(sModes[static_cast<int>(SModes::Inv)]), 0, 0, atype));
        connect(vec_btns.back(), SIGNAL(ClickServButton(QString)), SLOT(ProcessClickFuncModes(QString)));

        vec_btns.push_back(new QCalcWidget(new QCheckBox(sModes[static_cast<int>(SModes::Hyp)]), 0, 1, atype));
        connect(vec_btns.back(), SIGNAL(ClickServButton(QString)), SLOT(ProcessClickFuncModes(QString)));
        return;
    case Pnl::ServButtons:
        vec_btns.push_back(new QCalcWidget(new QPushButton(), 0, 0, atype, sServ[static_cast<int>(Serv::BackSpace)]));
        connect(vec_btns.back(), SIGNAL(ClickButton(QString)), SLOT(ProcessClickServ(QString)));

        vec_btns.push_back(new QCalcWidget(new QPushButton(), 0, 1, atype, sServ[static_cast<int>(Serv::CE)]));
        connect(vec_btns.back(), SIGNAL(ClickButton(QString)), SLOT(ProcessClickServ(QString)));

        vec_btns.push_back(new QCalcWidget(new QPushButton(), 0, 2, atype, "C", sServ[static_cast<int>(Serv::Esc)]));
        connect(vec_btns.back(), SIGNAL(ClickButton(QString)), SLOT(ProcessClickServ(QString)));
        return;
    default:
        return;
    }

    for(auto i = 0; i < max_i; ++i)
        for(auto j = 0; j < max_j; ++j)
        {
            index = max_j * i + j;            
            cw = new QCalcWidget(new QPushButton(), i, j, atype, s[index], s_v[index]);
            cw->SetTextColor(color);

            if(atype == Pnl::Mem)
                connect(cw, SIGNAL(ClickButton(QString)), SLOT(ProcessClickMem(QString)));
            else
                connect(cw, SIGNAL(ClickButton(QString)), SLOT(ProcessClick(QString)));

            if(atype == Pnl::Func)
            {
                cw->SetAltTexts(svAltFunc[index], sAltFunc[index]);
                cw->hyp = sHypInv[index] == "h";
                cw->inv = sHypInv[index] == "i";
            }
            if(atype == Pnl::Op)
            {
                cw->SetNopable(bOpNops[index]);
                cw->SetReplaceValue(sOpReps[index]);
            }


            vec_btns.push_back(cw);
        }
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::ResizeWidgets(int w, int h, Pnl atype)
{
    for (QCalcWidget* widget: vec_btns) { if(widget->GetType() == atype) widget->SetSize(w, h);}
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::SetSizeOfWidgets(int button_w, int button_h)
{
    int i_left, i_top, i_right, i_bottom;
    int func_button_h = GetHFButton(button_h);


    auto margins = this->contentsMargins();
    i_left = margins.left();
    i_top = margins.top();
    i_right = margins.right();
    i_bottom = margins.bottom();

    pb.setSizeButton(Pnl::Dig, button_w, button_h);

    switch(static_cast<CalcView>(settings.getSetting("appview").toInt()))
    {
    case CalcView::Original:
        pb.setSizeButton(Pnl::Op, button_w, button_h);
        pb.setSizeButton(Pnl::Abc, button_w, button_h);
        pb.getPanel(Pnl::Mem)->setFixedSize(pb.getPanel(Pnl::Abc)->width(), pb.getPanel(Pnl::Op)->height() - pb.getPanel(Pnl::Abc)->height() - spacing);
        wBottom->setFixedSize(pb.getPanel(Pnl::Dig)->width() + pb.getPanel(Pnl::Op)->width() + pb.getPanel(Pnl::Mem)->width() + spacing * 2,
                              pb.getPanel(Pnl::Dig)->height());

        button_func_w = (wBottom->width() - spacing * (pb.getCols(Pnl::Func) + 1)) / pb.getCols(Pnl::Func);
        pb.setSizeButton(Pnl::Func, button_func_w, func_button_h);
        pb.setSizeButton(Pnl::Scale, button_func_w, func_button_h);
        pb.setSizeButton(Pnl::FuncModes, button_func_w, func_button_h);
        pb.getPanel(Pnl::Drg)->setFixedSize(wBottom->width() - pb.getPanel(Pnl::Scale)->width() - spacing,
                                            pb.getPanel(Pnl::Scale)->height());

        wMode->setFixedSize(wBottom->width(), pb.getPanel(Pnl::Scale)->height() + pb.getPanel(Pnl::FuncModes)->height() + spacing);
        ResizeWidgets(button_w, pb.getPanel(Pnl::FuncModes)->height(), Pnl::ServButtons);

        this->setFixedSize(wBottom->width() + i_left + i_right,
                           wBottom->height() + pb.getPanel(Pnl::Func)->height() + wMode->height() +
                           (isLogList()? cbxlogList->height() + spacing: 0) +
                           wDisplay->height() + spacing * 4 + MenuBar->height() + i_top + i_bottom);
        break;
    case CalcView::Simple:
        pb.setSizeButton(Pnl::Op, button_w, button_h, button_w + spacing);
        wBottom->setFixedSize(pb.getPanel(Pnl::Dig)->width() + pb.getPanel(Pnl::Op)->width() + spacing, pb.getPanel(Pnl::Dig)->height());

        wMode->setFixedSize(wBottom->width(), func_button_h + i_top + i_bottom);
        ResizeWidgets(button_w, wMode->height(), Pnl::ServButtons);

        this->setFixedSize(wBottom->width() + i_left + i_right,
                           wBottom->height() + wMode->height() +
                           (isLogList()? cbxlogList->height() + spacing: 0) +
                           wDisplay->height() +  spacing * 3 + MenuBar->height() + i_top + i_bottom);

        break;
    case CalcView::Programmable:
        //
        break;
    }
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::ResizeAll(unsigned new_button_w, unsigned new_button_h)
{
    settings.setSetting("button_width", new_button_w);
    settings.setSetting("button_height", new_button_h);


    int logical_w = (QPaintDevice::logicalDpiX() * settings.getSetting("button_width").toInt()) / DEFAULT_DPI;
    int logical_h = (QPaintDevice::logicalDpiY() * settings.getSetting("button_height").toInt()) / DEFAULT_DPI;

    ResizeWidgets(logical_w, logical_h, Pnl::Dig);
    ResizeWidgets(logical_w, logical_h, Pnl::Op);
    ResizeWidgets(logical_w, logical_h, Pnl::Mem);
    ResizeWidgets(logical_w, logical_h, Pnl::Abc);

    SetSizeOfWidgets(logical_w, logical_h);

    ResizeWidgets(button_func_w, GetHFButton(logical_h), Pnl::Func);
    ResizeWidgets(button_func_w + spacing, GetHFButton(logical_h), Pnl::Scale);
    ResizeWidgets(button_func_w + spacing, GetHFButton(logical_h), Pnl::Drg);
    ResizeWidgets(button_func_w + spacing, GetHFButton(logical_h), Pnl::FuncModes);

    posMousePress.setX(logical_w / 2);
    posMousePress.setY(logical_h / 3);
    ReCreateMouseEvents();
}


//----------------------------------------------------------------------------------------------------------------------
bool MainWidget::AddToken(const QString& stok)
{
    bool res;
    QString s = stok;
    res = parser->AddToken(&s);
    if(!res)
        Alert();
    else
        SaveExpression(parser->GetExpression());
    return res;
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::ProcessClick(const QString& sButtonValue)
{
    bool accessRun;
    if(sButtonValue == "=")
    {
        accessRun = parser->Run();
        if(accessRun)
            UpdateDisplay(ud::Result);
        else
            UpdateDisplay(ud::Errors);
    }
    else
    {
        QString s = sButtonValue;
        if(s == "^2")                            //Для перевода степени в нужную систему счисления
            s = "^" + parser->DoubleToString(2);
        if(s == "^3")
            s = "^" + parser->DoubleToString(3);

        AddToken(s);
        UpdateDisplay();        
    }
}


//----------------------------------------------------------------------------------------------------------------------
bool MainWidget::SetMemory(QString value, CalcParser* p)
{
  CalcParser* l_parser = nullptr;
  bool result = false;

  l_parser = (p == nullptr? new CalcParser(&value): p);

  if(l_parser->Run())
  {
      inMemory = l_parser->GetResult();
      if(value == "")
          lblMem->setText("");
      else
          lblMem->setText(l_parser->DoubleToString(inMemory, 6));
      result = true;
  }

  if(p == nullptr)
      delete l_parser;

  return result;
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::LoadExpression()
{
    QString exp = settings.getSetting("expression").toString();
    if(!exp.isEmpty() && exp != "0")
    {
        without_logging = true;
        pasteExpression(exp);
        without_logging = false;
    }
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::SaveExpression(const QString &value)
{
    settings.setSetting("expression", value);
}



//----------------------------------------------------------------------------------------------------------------------
void MainWidget::ProcessClickMem(const QString& sButtonValue)
{
    QString stok = sButtonValue;

    if(stok == sMem[0])
    {
        if(SetMemory("0", parser))
            SaveMemory(parser->GetExpression());
        else
            UpdateDisplay(ud::Errors);

    }
    if(stok == sMem[1])
    {
        if(lblMem->text().isEmpty())
            return;

        QString sMemory = parser->DoubleToString(fabs(inMemory), PRECISION_FOR_DOUBLE);
        if(AddToken(sMemory))
        {
            if(inMemory < 0)
                parser->AddPrefixOp("-");
        }

        UpdateDisplay();
    }
    if(stok == sMem[2])
    {
        lblMem->setText("");
        inMemory = 0;
        SaveMemory("");
    }

}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::ProcessClickScale(const QString& sButtonValue)
{
    QString scale = sButtonValue;

    if(scale == "Hex")
        parser->SetScale(16);
    if(scale == "Dec")
        parser->SetScale(10);
    if(scale == "Oct")
        parser->SetScale(8);
    if(scale == "Bin")
        parser->SetScale(2);
    UpdateDisplay();
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::ProcessClickServ(const QString& sButtonValue)
{
    if(sButtonValue == "Back")
        parser->ToBack();
    if(sButtonValue == "CE")
        parser->ToBack(false);
    if(sButtonValue == "ESC")
        parser->SetParams(nullptr, parser->Scale(), parser->DRG());

    UpdateDisplay();
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::ProcessClickDRG(const QString& sButtonValue)
{

    Drg drg_mode = Drg::Rad;

    if(sButtonValue == "Deg")
        drg_mode = Drg::Deg;
    if(sButtonValue == "Rad")
        drg_mode = Drg::Rad;
    if(sButtonValue == "Grad")
        drg_mode = Drg::Grad;
    parser->SetDRG(drg_mode);

    UpdateDisplay();
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::ProcessClickFuncModes(const QString& sModeValue)
{
    QCalcWidget* w = FindButtonByValue(sModeValue);
    if(!w) return;

    stFuncMode FM = {sModeValue, reinterpret_cast<QCheckBox*>(reinterpret_cast<QCalcWidget*>(w->widget))->isChecked()};
    std::for_each(vec_btns.begin(), vec_btns.end(), bind2nd(setHypInv(), FM));
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::SetMode(SModes mode, bool on)
{
    std::vector<QCalcWidget*> btns_fm;
    btns_fm.resize(std::count_if(vec_btns.begin(), vec_btns.end(), bind2nd(checkBtnType(), Pnl::FuncModes)));
    std::copy_if(vec_btns.begin(), vec_btns.end(), btns_fm.begin(), bind2nd(checkBtnType(), Pnl::FuncModes));

    if(static_cast<int>(mode) < static_cast<int>(SModes::Inv) || static_cast<int>(mode) > static_cast<int>(SModes::Hyp) ) return;
    QCheckBox *cb = reinterpret_cast<QCheckBox*>(btns_fm.at(static_cast<int>(mode))->widget);
    if(cb->isChecked() != on)
        cb->click();        
}



//----------------------------------------------------------------------------------------------------------------------
void MainWidget::UpdateDisplay(ud how_update)
{
    QString expression;
    QString errors;
    QString result;

    expression = parser->GetExpression();
    errors = parser->listErrors();

    wDisplay->setExpression(parser->GetExpression("", true));

    if(how_update == ud::Errors)
        wDisplay->setResult(errors);
    else
        if(how_update == ud::Result)
        {
            result = parser->DoubleToString(parser->GetResult(), PRECISION_FOR_DOUBLE);
            wDisplay->setResult("= " + result);

            if(isLoging() && !without_logging)
                loger.Add(expression +  " = " + result);
        }
    else
            if(how_update == ud::Empty)
                wDisplay->setResult("");

    if(!lblMem->text().isEmpty())
        lblMem->setText(parser->DoubleToString(inMemory, 6));
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::Alert(void)
{
    if(!bPasting)
        QApplication::beep();
}


//----------------------------------------------------------------------------------------------------------------------
QCalcWidget* MainWidget::FindButtonByValue(QString value)
{
    auto it = std::find_if(vec_btns.begin(), vec_btns.end(), bind2nd(buttonIsValue(), value));
    return *it;
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::AssignKeyToButton(QString button_value, int key, unsigned int mod)
{
    QCalcWidget *calcwidget = nullptr;
    sKeyMod keymod = {key, mod};

    calcwidget = FindButtonByValue(button_value);
    if(calcwidget)
    {
        map_keys[keymod] = calcwidget;
        map_val_key[button_value] = keymod;

        if(calcwidget->AltValue() != button_value)
            map_val_key[calcwidget->AltValue()] = keymod;
    }
}


//----------------------------------------------------------------------------------------------------------------------
void MainWidget::DefaultKeysMap(void)
{
    AssignKeyToButton("0", Qt::Key_0);
    AssignKeyToButton("1", Qt::Key_1);
    AssignKeyToButton("2", Qt::Key_2);
    AssignKeyToButton("3", Qt::Key_3);
    AssignKeyToButton("4", Qt::Key_4);
    AssignKeyToButton("5", Qt::Key_5);
    AssignKeyToButton("6", Qt::Key_6);
    AssignKeyToButton("7", Qt::Key_7);
    AssignKeyToButton("8", Qt::Key_8);
    AssignKeyToButton("9", Qt::Key_9);

    AssignKeyToButton(".", Qt::Key_Comma);
    AssignKeyToButton(".", Qt::Key_Period);
    AssignKeyToButton("+/-", Qt::Key_F9);
    AssignKeyToButton("*", Qt::Key_Asterisk);
    AssignKeyToButton("+", Qt::Key_Plus);
    AssignKeyToButton("-", Qt::Key_Minus);
    AssignKeyToButton("-", Qt::Key_Underscore, Qt::ShiftModifier);
    AssignKeyToButton("/", Qt::Key_Slash);

    AssignKeyToButton("!", Qt::Key_Exclam);
    AssignKeyToButton("~", Qt::Key_AsciiTilde);
    AssignKeyToButton("%", Qt::Key_Percent);
    AssignKeyToButton("&", Qt::Key_Ampersand);
    AssignKeyToButton("|", Qt::Key_Bar);
    AssignKeyToButton("<<", Qt::Key_Less);
    AssignKeyToButton(">>", Qt::Key_Greater);

    AssignKeyToButton("=", Qt::Key_Return);
    AssignKeyToButton("=", Qt::Key_Enter);
    AssignKeyToButton("=", Qt::Key_Equal);
    AssignKeyToButton("Back", Qt::Key_Backspace);
    AssignKeyToButton("CE", Qt::Key_Delete);
    AssignKeyToButton("ESC", Qt::Key_Escape);

    AssignKeyToButton("A", Qt::Key_A);
    AssignKeyToButton("B", Qt::Key_B);
    AssignKeyToButton("C", Qt::Key_C);
    AssignKeyToButton("D", Qt::Key_D);
    AssignKeyToButton("E", Qt::Key_E);
    AssignKeyToButton("F", Qt::Key_F);

    AssignKeyToButton("(", Qt::Key_ParenLeft);
    AssignKeyToButton(")", Qt::Key_ParenRight);
    AssignKeyToButton("sin(", Qt::Key_S, Qt::ControlModifier);
    AssignKeyToButton("cos(", Qt::Key_C, Qt::ControlModifier);
    AssignKeyToButton("tan(", Qt::Key_T, Qt::ControlModifier);
    AssignKeyToButton("ctan(", Qt::Key_O, Qt::ControlModifier);
    AssignKeyToButton("asin(", Qt::Key_S, Qt::ShiftModifier);
    AssignKeyToButton("acos(", Qt::Key_C, Qt::ShiftModifier);
    AssignKeyToButton("atan(", Qt::Key_T, Qt::ShiftModifier);
    AssignKeyToButton("actan(", Qt::Key_O, Qt::ShiftModifier);
    AssignKeyToButton("pi", Qt::Key_P);
    AssignKeyToButton("ln(", Qt::Key_N, Qt::ControlModifier);
    AssignKeyToButton("log(", Qt::Key_L, Qt::ControlModifier);
    AssignKeyToButton("exp(", Qt::Key_E, Qt::ControlModifier);
    AssignKeyToButton("ln(", Qt::Key_N, Qt::ControlModifier);
    AssignKeyToButton("1/x", Qt::Key_Slash, Qt::ControlModifier);
    AssignKeyToButton("1/x", Qt::Key_Slash, Qt::ControlModifier | Qt::KeypadModifier);
    AssignKeyToButton("exp", Qt::Key_X, Qt::ControlModifier);
    AssignKeyToButton("^2", Qt::Key_2, Qt::ControlModifier);
    AssignKeyToButton("^3", Qt::Key_3, Qt::ControlModifier);
    AssignKeyToButton("^", Qt::Key_AsciiCircum);
    AssignKeyToButton("^", Qt::Key_6, Qt::ControlModifier);
    AssignKeyToButton("fact(", Qt::Key_F, Qt::ControlModifier);
    AssignKeyToButton("dms(", Qt::Key_M);

    AssignKeyToButton("Hex", Qt::Key_F5);
    AssignKeyToButton("Dec", Qt::Key_F6);
    AssignKeyToButton("Oct", Qt::Key_F7);
    AssignKeyToButton("Bin", Qt::Key_F8);

    AssignKeyToButton("Deg", Qt::Key_F2);
    AssignKeyToButton("Rad", Qt::Key_F3);
    AssignKeyToButton("Grad", Qt::Key_F4);

    AssignKeyToButton("Hyp", Qt::Key_H);
    AssignKeyToButton("Inv", Qt::Key_I);

    AssignKeyToButton("MS", Qt::Key_M, Qt::ControlModifier);
    AssignKeyToButton("MR", Qt::Key_R, Qt::ControlModifier);
    AssignKeyToButton("MC", Qt::Key_E, Qt::ShiftModifier);

    //AssignKeyToButton("exp(", Qt::Key_P, Qt::ControlModifier); //Добавление функции без кнопки на окне.
}

