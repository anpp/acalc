#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "buttexts.h"
#include "dialogsettings.h"
#include "functors.h"
#include <qlocale.h>

QString sViews[] = {QObject::tr("Original"), QObject::tr("Simple"), QObject::tr("Programmable")};
QString sLanguages[] = {QObject::tr("English"), QObject::tr("Russian")};
QString sShortLanguages[] = {"en", "ru"};

inline unsigned GetHFButton(unsigned h)
{
    return (int)h - 10 > 5? h - 10: 5;
}



MainWidget::MainWidget(QWidget *parent) :
        QWidget(parent, Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint),
    ui(new Ui::MainWidget), settings("anp", "acalc")
{
    ui->setupUi(this);

    mePress = NULL;
    meRelease = NULL;

    settings.beginGroup("/appearance");
    button_w = settings.value("/button_width", WIDTH_BUT).toInt();
    button_h = settings.value("/button_height", HEIGHT_BUT).toInt();
    viewCalc = (VIEWCALC)settings.value("/view", ORIGINAL).toInt();
    spacing = SPACING;
    settings.endGroup();

    this->setContentsMargins(spacing / 2, 0, spacing / 2, spacing / 2);

    curr_widget = NULL;
    bPasting = false;

    parser = new CalcParser();

    //QApplication::setStyle("plastique");
    QApplication::setStyle("fusion");
    shape = QFrame::WinPanel | QFrame::Sunken;
    colorDigits = Qt::blue;
    colorOps = Qt::red;
    colorAbc.setRgb(0, 64, 128);
    colorMem.setRgb(128, 0, 128);
    colorFunc = Qt::black;

    CreateMenus();    
    CreateWidgets();
    InitLocale();
    DefaultKeysMap();


    parser->SetVariable("default_color", "#000000", STRING);
    parser->SetVariable("number_color", String(colorDigits.name().toStdString().c_str()), STRING);
    parser->SetVariable("function_color", String(colorFunc.name().toStdString().c_str()), STRING);
    parser->SetVariable("delimiter_color", String(colorOps.name().toStdString().c_str()), STRING);

    SendKey(Qt::Key_F3); //Rad
    SendKey(Qt::Key_F6); //Dec

    SetView(viewCalc);

    UpdateDisplay();
    this->setFocus();
}

MainWidget::~MainWidget()
{
    settings.beginGroup("/appearance");
    settings.setValue("/button_width", button_w);
    settings.setValue("/button_height", button_h);
    settings.setValue("/view", viewCalc);
    settings.endGroup();

    delete ui;
    delete parser;
    delete mePress;
    delete meRelease;
}



void MainWidget::ReCreateMouseEvents()
{
    delete mePress;
    delete meRelease;

    mePress = new QMouseEvent(QEvent::MouseButtonPress, posMousePress, Qt::LeftButton,
                                           Qt::LeftButton, Qt::NoModifier);
    meRelease = new QMouseEvent(QEvent::MouseButtonRelease, posMousePress, Qt::LeftButton,
                                      Qt::LeftButton, Qt::NoModifier);
}



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

    for(unsigned int i = 0; i < sizeof(sViews) / sizeof(QString); ++i)
    {
        ActionViews.append(MenuView->addAction(""));
        ActionViews.last()->setCheckable(true);
    }
    ActionViews.last()->setEnabled(false);
    connect(MenuView, SIGNAL(triggered(QAction*)), SLOT(slotView(QAction*)));

    for(unsigned int i = 0; i < sizeof(sLanguages) / sizeof(QString); ++i)
    {
        ActionLanguages.append(MenuLanguages->addAction(""));
        ActionLanguages.last()->setCheckable(true);
    }
    connect(MenuLanguages, SIGNAL(triggered(QAction*)), SLOT(slotLanguage(QAction*)));


    ActionCopy = MenuEdit->addAction("", this, SLOT(slotCopy(void)), Qt::CTRL + Qt::Key_Insert);
    ActionPaste = MenuEdit->addAction("", this, SLOT(slotPaste(void)), Qt::SHIFT + Qt::Key_Insert);
    MenuEdit->addSeparator();
    MenuEdit->addMenu(MenuLanguages);
    ActionSettings = MenuEdit->addAction("", this, SLOT(slotSettings(void)));

    ActionContextHelp = MenuHelp->addAction("", this, SLOT(slotEnterWhatIsThis(void)), Qt::SHIFT + Qt::Key_F1);
    ActionAbout = MenuHelp->addAction("", this, SLOT(slotAbout(void)));

    MenuOnButton = new QMenu(this);
    ActionWhatIs = MenuOnButton->addAction("", this, SLOT(slotMenuWhatIsThis(void)));
}


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
    ActionContextHelp->setText(tr("Contex help"));
    ActionWhatIs->setText(tr("What is this?"));
    ActionAbout->setText(tr("About..."));


    foreach (QAction *action, ActionViews) {
        action->setText(QObject::tr(sViews[ActionViews.indexOf(action)].toStdString().c_str()));
    }
    foreach (QAction *action, ActionLanguages) {
        action->setText(tr(sLanguages[ActionLanguages.indexOf(action)].toStdString().c_str()));
    }
    foreach (QCalcWidget *w, vec_btns) {w->LoadWhatIsText();}
}


void MainWidget::slotView(QAction* action)
{
    if(action == NULL) return;
    SetView(ActionViews.indexOf(action));
}


void MainWidget::slotLanguage(QAction* action)
{
    if(action == NULL) return;
    SetLocale(ActionLanguages.indexOf(action));
}



void MainWidget::slotAbout(void)
{
    QMessageBox::about(this, tr("About"), tr("AboutProgram"));
}


void MainWidget::slotCopy(void)
{
  QString s = GetExpression();
  if(!wResult->text().isEmpty())
  {
      if(parser->Run())
          s = s + " = " + NumberToString(parser->GetResult());
      else
          s = s + " " + GetErrors();
  }
  QApplication::clipboard()->setText(s);
}


void MainWidget::slotPaste(void)
{
#ifndef _QT4
    std::string value, num_val;
    std::string pasteString = QApplication::clipboard()->text().toStdString();
    if(pasteString.empty() || "" == pasteString)
#else
    QString value, num_val;
    QString pasteString = QApplication::clipboard()->text();
    if(pasteString.isEmpty() || "" == pasteString)
#endif
        return;

    SendKey(Qt::Key_Escape);

    TokenList Tokens;
    Token tok;
    CalcParser tempParser;
    sKeyMod km = {0, 0};
    QCalcWidget *w = NULL;

    tempParser.SetParams(&pasteString, 0, parser->DRG());
    Tokens = tempParser.RefTokens();

    bPasting = true;
    for(TokenList::iterator it = Tokens.begin(); it != Tokens.end(); ++it)
    {
        tok = *it;
        value = tok.Value();
        if(tok.Type() == UNK || tok.Type() == ERR) continue;
        if(tok.Type() == NUMBER)
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

#ifndef _QT4
                km = FindKeyByValue(QString::fromStdString(num_val));
#else
                km = FindKeyByValue(num_val);
#endif
                SendKey(km.key, km.mod);
            }

        }
        else
        {
            SetMode(HYP, false);
            SetMode(INV, false);
#ifndef _QT4
            km = FindKeyByValue(QString::fromStdString(value));
#else
            km = FindKeyByValue(value);
#endif
            if(km.key == 0 && km.mod == 0) //Если не найдено сочетание клавиш для функции
                continue;

            w = FindWidgetByKey(km);
            if(w)
            {
#ifndef _QT4
                if(tok.Type() == FUNCTION && tok.Value() != w->Value().toStdString())
#else
                if(tok.Type() == FUNCTION && tok.Value() != w->Value())
#endif
                {
                    if(w->Hypable())
                        SetMode(HYP, true);
                    else
                        if(w->Invable())
                            SetMode(INV, true);
                }
                ClickToWidget(w->widget);
            }

            if(tok.Value() == "=")
                break;
        }
    }
    bPasting = false;
}


void MainWidget::slotSettings(void)
{
  DialogSettings settings;
  settings.adjustSize();
  settings.setFixedSize(settings.width(), settings.height());
  settings.exec();
}



void
MainWidget::SendKey(int Key, int Mod)
{
    QKeyEvent *ke = new QKeyEvent(QEvent::KeyPress, Key, (Qt::KeyboardModifiers)Mod);
    QApplication::sendEvent(this, ke);
    delete ke;
}


void MainWidget::ClickToWidget(QWidget *widget, int msec)
{
    QTime time;
    if(widget)
    {
        if(!widget->isVisible())
            ((QAbstractButton*)(widget))->click();
        else
        {
            if(!(mePress && meRelease)) return;
            QApplication::sendEvent(widget, mePress);
            time.start();
            for(; time.elapsed() < msec;){}
            QApplication::sendEvent(widget, meRelease);
        }
    }
}


void MainWidget::focusOutEvent(QFocusEvent * fe)
{
    this->setFocus();
    QWidget::focusOutEvent(fe);
}


sKeyMod MainWidget::FindKeyByValue(QString value)
{
    sKeyMod km = {0, 0};
    QMap<QString, sKeyMod>::iterator mit = map_val_key.find(value);
    if(mit != map_val_key.end())
        km = *mit;
    return km;
}



QCalcWidget* MainWidget::FindWidgetByKey(sKeyMod km)
{
    QMap<sKeyMod, QCalcWidget*>::iterator mit = map_keys.find(km);
    if(mit != map_keys.end())
        return *mit;
    return NULL;
}


void MainWidget::keyPressEvent(QKeyEvent* pe)
{
    QCalcWidget *cw;
    sKeyMod km = {pe->key(), pe->modifiers()};

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


void MainWidget::mousePressEvent(QMouseEvent *mpe)
{
    mpe->accept();
    curr_widget = NULL;

    curr_widget = this->childAt(mpe->pos());

    curr_GlobalPos = mpe->globalPos();
    if(mpe->button() == Qt::RightButton && !QWhatsThis::inWhatsThisMode())
        if(curr_widget)
            MenuOnButton->exec(mpe->globalPos());

}

void MainWidget::slotMenuWhatIsThis(void)
{
    if(curr_widget)
        QWhatsThis::showText(curr_GlobalPos, curr_widget->whatsThis());
}


void MainWidget::slotEnterWhatIsThis(void)
{
    QWhatsThis::enterWhatsThisMode();
}


bool MainWidget::event(QEvent *e)
{
    if((e->type() == QEvent::Leave || e->type() == QEvent::LeaveWhatsThisMode) && curr_widget)
        curr_widget->update();

    return QWidget::event(e);
}


void MainWidget::InitLocale()
{

#ifndef _QT4 // для парсера, если он собирается с gettext, а не с Qt
    setlocale(LC_ALL, "");
    #ifndef _UNIX
        bind_textdomain_codeset(PACKAGE, "utf-8");
        bindtextdomain(PACKAGE, QString(qApp->applicationDirPath() + "/locale").toStdString().c_str());
    #else
        bind_textdomain_codeset(PACKAGE, "utf-8");
        bindtextdomain(PACKAGE, "./locale");
    #endif
        textdomain(PACKAGE);
#endif
    qtTrans = new QTranslator(this);
    qtTransPopup = new QTranslator(this);
    qtTransErrors = new QTranslator(this);

    elangs lang = EN;
    QLocale locale;
    if(locale.language() == QLocale::Russian)
        lang = RU;

    SetLocale(lang);
}

void MainWidget::SetLocale(int indexLang)
{
    LoadLocale(sShortLanguages[indexLang]);

    int index = 0;
    foreach(QAction *action, ActionLanguages){
        action->setChecked(indexLang == index++);
    }
}

void MainWidget::SetView(int indexView)
{
    viewCalc = (VIEWCALC)indexView;
    if(!InitLayouts())
        QApplication::exit(-1);

    int index = 0;
    foreach(QAction *action, ActionViews){
        action->setChecked(indexView == index++);
    }
}

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



bool MainWidget::InitLayouts()
{
    QCalcWidget* rw;
    QString rv;

    FreeLayouts();
    switch(viewCalc)
    {
    case ORIGINAL:
        foreach (QCalcWidget* widget, vec_btns) {
            if(widget->GetType() == NOP) widget->SetType(widget->GetSavedType());
            if(widget->IsReplace()) widget->UnReplace();
        }
        LayoutOriginal();
        break;
    case SIMPLE:
        foreach (QCalcWidget* widget, vec_btns) {
            rv = widget->GetReplaceValue();
            if("" != rv)
            {
                rw = FindButtonByValue(rv);
                if(rw) rw->Replace(widget);
            }

            if(widget->IsNopable()) widget->SetType(NOP);
        }
        LayoutSimple();
        break;
    case PROGRAMMABLE:
        LayoutProgrammable();
        break;
    default:
        return false;
    }
    ResizeAll();
    return true;
}


void MainWidget::FreeLayouts(void)
{
    wAbc->setVisible(viewCalc == ORIGINAL);
    wMem->setVisible(viewCalc == ORIGINAL);
    wCentral->setVisible(viewCalc == ORIGINAL);
    wScale->setVisible(viewCalc == ORIGINAL);
    wDRG->setVisible(viewCalc == ORIGINAL);
    wFuncModes->setVisible(viewCalc == ORIGINAL);

    QString classname;

    foreach(QWidget* w, QApplication::allWidgets())
    {
        classname = w->metaObject()->className();
        if("QPushButton" != classname)
            delete w->layout();
    }
}


void MainWidget::FillLayoutWidgets(QLayout *l, pnl atype, bool bGrid)
{
    stTypeLayout TL = {l, atype, bGrid};
    std::for_each(vec_btns.begin(), vec_btns.end(), bind2nd(setBtnLayout(), TL));
}


void MainWidget::LayoutOriginal(void)
{
    QGridLayout *lFuncs = new QGridLayout();
    QGridLayout *lDigits = new QGridLayout();
    QGridLayout *lOps = new QGridLayout();
    QGridLayout *lMem = new QGridLayout();
    QGridLayout *lAbc = new QGridLayout();
    QVBoxLayout *lMain = new QVBoxLayout();
    QVBoxLayout *lMenu = new QVBoxLayout();
    QHBoxLayout *lBottom = new QHBoxLayout();
    QVBoxLayout *lMemAbc = new QVBoxLayout();
    QGridLayout *lDisplay = new QGridLayout();

    lMain->setContentsMargins(0, 0, 0, 0);
    lMain->setSpacing(spacing);

    lMenu->setContentsMargins(0, 0, 0, 0);
    lMenu->setSpacing(0);
    lMenu->addWidget(MenuBar);


    lMain->addLayout(lMenu);
    lMain->addWidget(wDisplay);
    lMain->addWidget(wMode);
    lMain->addWidget(wCentral);
    lMain->addWidget(wBottom);


    lFuncs->setMargin(spacing);
    lFuncs->setSpacing(spacing);
    FillLayoutWidgets(lFuncs, FUNC);


    lMem->setMargin(spacing);
    lMem->setSpacing(spacing);
    FillLayoutWidgets(lMem, MEM);


    lAbc->setMargin(spacing);
    lAbc->setSpacing(spacing);
    FillLayoutWidgets(lAbc, ABC);


    lMemAbc->setMargin(0);
    lMemAbc->setSpacing(spacing);
    lMemAbc->addWidget(wMem);
    lMemAbc->addWidget(wAbc);



    lBottom->setMargin(0);
    lBottom->setSpacing(spacing);
    lBottom->addWidget(wDigits, Qt::AlignLeft);
    lBottom->addWidget(wOps, Qt::AlignLeft);
    lBottom->addLayout(lMemAbc);



    lDigits->setMargin(spacing);
    lDigits->setSpacing(spacing);
    FillLayoutWidgets(lDigits, DIG);

    lOps->setMargin(spacing);
    lOps->setSpacing(spacing);
    FillLayoutWidgets(lOps, OP);


    InitModesLayouts();

    lDisplay->setMargin(0);
    lDisplay->setSpacing(0);
    lDisplay->addWidget(Display, 0, 0);
    lDisplay->addWidget(wResult, 1, 0);

    wCentral->setLayout(lFuncs);
    wMem->setLayout(lMem);
    wAbc->setLayout(lAbc);
    wBottom->setLayout(lBottom);
    wDigits->setLayout(lDigits);
    wOps->setLayout(lOps);
    wDisplay->setLayout(lDisplay);

    this->setLayout(lMain);
}



void MainWidget::LayoutSimple(void)
{
    QGridLayout *lDisplay = new QGridLayout();
    QVBoxLayout *lMain = new QVBoxLayout();
    QVBoxLayout *lMenu = new QVBoxLayout();
    QGridLayout *lDigits = new QGridLayout();
    QGridLayout *lOps = new QGridLayout();
    QHBoxLayout *lBottom = new QHBoxLayout();
    QHBoxLayout *lMode = new QHBoxLayout();

    //lMain->setContentsMargins(spacing, 0, spacing, spacing);
    lMain->setContentsMargins(0, 0, 0, 0);
    lMain->setSpacing(spacing);

    lMenu->setContentsMargins(0, 0, 0, 0);
    lMenu->setSpacing(0);
    lMenu->addWidget(MenuBar);

    lMain->addLayout(lMenu);
    lMain->addWidget(wDisplay);
    lMain->addWidget(wMode);
    lMain->addWidget(wBottom);

    lBottom->setMargin(0);
    lBottom->setSpacing(spacing);
    lBottom->addWidget(wDigits, Qt::AlignLeft);
    lBottom->addWidget(wOps, Qt::AlignLeft);

    lDigits->setMargin(spacing);
    lDigits->setSpacing(spacing);
    FillLayoutWidgets(lDigits, DIG);

    lOps->setMargin(spacing);
    lOps->setSpacing(spacing);
    FillLayoutWidgets(lOps, OP);

    lDisplay->setMargin(0);
    lDisplay->setSpacing(0);
    lDisplay->addWidget(Display, 0, 0);
    lDisplay->addWidget(wResult, 1, 0);

    lMode->setMargin(0);
    lMode->setSpacing(spacing);
    lMode->addStretch();
    FillLayoutWidgets(lMode, SERVBUTTONS, false);

    wBottom->setLayout(lBottom);
    wDigits->setLayout(lDigits);
    wOps->setLayout(lOps);
    wMode->setLayout(lMode);
    wDisplay->setLayout(lDisplay);
    this->setLayout(lMain);
}


void MainWidget::LayoutProgrammable(void)
{
//
}



void MainWidget::InitModesLayouts()
{
    QHBoxLayout *lModeBottom = new QHBoxLayout();
    QHBoxLayout *lScale = new QHBoxLayout();
    QHBoxLayout *lDrg = new QHBoxLayout();
    QVBoxLayout *lMode = new QVBoxLayout();
    QHBoxLayout *lModeTop = new QHBoxLayout();
    QHBoxLayout *lFuncModes = new QHBoxLayout();

    lMode->setMargin(0);
    lMode->setSpacing(spacing);
    lMode->addLayout(lModeTop);
    lMode->addLayout(lModeBottom);

    lModeBottom->setMargin(0);
    lModeBottom->setSpacing(spacing);
    lModeBottom->addWidget(wScale);
    lModeBottom->addWidget(wDRG);

    lScale->setContentsMargins(spacing, 0, 0, 0);
    lScale->setSpacing(0);
    FillLayoutWidgets(lScale, SCALE, false);

    lDrg->setContentsMargins(0, 0, 0, 0);
    lDrg->setSpacing(0);
    FillLayoutWidgets(lDrg, DRG, false);


    lModeTop->setMargin(0);
    lModeTop->setSpacing(spacing);
    lModeTop->addWidget(wFuncModes);
    lModeTop->addStretch();
    FillLayoutWidgets(lModeTop, SERVBUTTONS, false);


    lFuncModes->setContentsMargins(spacing, 0, 0, 0);
    lFuncModes->setSpacing(0);
    FillLayoutWidgets(lFuncModes, FUNCMODES, false);

    wScale->setLayout(lScale);
    wDRG->setLayout(lDrg);
    wFuncModes->setLayout(lFuncModes);
    wMode->setLayout(lMode);
}




void MainWidget::CreateWidgets()
{
    wDisplay = new QFrame(this);
    wMode = new QFrame(this);
    wCentral = new QFrame(this);
    wBottom = new QFrame(this);
    wDigits = new QFrame(wBottom);
    wOps = new QFrame(wBottom);
    wMem = new QFrame(wBottom);
    wAbc = new QFrame(wBottom);
    wScale = new QFrame(wMode);
    wDRG = new QFrame(wMode);
    wFuncModes = new QFrame(wMode);
    Display = new QLabel(this);
    wResult = new QLabel(this);

    QFont dispFont("monospace", 10);
    Display->setFont(dispFont);
    Display->setAlignment(Qt::AlignRight);
    Display->setStyleSheet("QLabel {background-color: white}");

    //dispFont.setPointSize(10);
    wResult->setFont(dispFont);
    wResult->setAlignment(Qt::AlignRight);
    wResult->setStyleSheet("QLabel {background-color: white}");

    wBottom->setContentsMargins(0, 0, 0, 0);
    wMode->setContentsMargins(0, 0, 0, 0);
    wDisplay->setContentsMargins(0, 0, 0, 0);

    wCentral->setFrameStyle(shape);
    wDigits->setFrameStyle(shape);
    wOps->setFrameStyle(shape);
    wMem->setFrameStyle(shape);
    wAbc->setFrameStyle(shape);
    wScale->setFrameStyle(shape);
    wDRG->setFrameStyle(shape);
    wFuncModes->setFrameStyle(shape);
    wDisplay->setFrameStyle(shape);

    CreateButtons(DIG);
    CreateButtons(OP);
    CreateButtons(MEM);
    CreateButtons(ABC);
    CreateButtons(FUNC);
    CreateButtons(SCALE);
    CreateButtons(DRG);
    CreateButtons(FUNCMODES);
    CreateButtons(SERVBUTTONS);
}

void MainWidget::CreateButtons(pnl atype)
{
    QString *s;
    QString *s_v;
    int index;

    QCalcWidget *cw;
    unsigned max_i;
    unsigned max_j;

    QColor *color;
    std::vector<QCalcWidget*>::iterator it;

    switch(atype)
    {
    case DIG:
    {
        max_i = 4;
        max_j = 3;
        s = sDigits;
        s_v = svDigits;
        color = &colorDigits;
        break;
    }
    case OP:
    {
        max_i = 4;
        max_j = 3;
        s = sOperators;
        s_v = svOperators;
        color = &colorOps;
        break;
    }
    case ABC:
    {
        max_i = 2;
        max_j = 3;
        s = sAbc;
        s_v = sAbc;
        color = &colorAbc;
        break;
    }
    case MEM:
    {
        max_i = 1;
        max_j = 3;
        s = sMem;
        s_v = sMem;

        lblMem = new QLabel();
        lblMem->setFrameStyle(QFrame::WinPanel);
        lblMem->setAlignment(Qt::AlignCenter);
        vec_btns.push_back(new QCalcWidget(lblMem, 1, 0, atype, "", "", 1, 3, true));
        color = &colorMem;
        break;
    }
    case FUNC:
    {
        max_i = 3;
        max_j = 7;
        s = sFunc;
        s_v = svFunc;
        color = &colorFunc;
        break;
    }
    case SCALE:
        vec_btns.push_back(new QCalcWidget(new QRadioButton(sScales[SHEX]), 0, 0, atype));
        vec_btns.push_back(new QCalcWidget(new QRadioButton(sScales[SDEC]), 0, 1, atype));
        vec_btns.push_back(new QCalcWidget(new QRadioButton(sScales[SOCT]), 0, 2, atype));
        vec_btns.push_back(new QCalcWidget(new QRadioButton(sScales[SBIN]), 0, 3, atype));
        it = vec_btns.begin();
        for(; it != vec_btns.end(); ++it)
        {
            cw = *it;
            if(cw->GetType() == atype)
                connect(cw, SIGNAL(ClickServButton(QString)), SLOT(ProcessClickScale(QString)));
        }
        return;
    case DRG:
        vec_btns.push_back(new QCalcWidget(new QRadioButton(sDrg[DDEG]), 0, 0, atype));
        connect(vec_btns.back(), SIGNAL(ClickServButton(QString)), SLOT(ProcessClickDRG(QString)));

        vec_btns.push_back(new QCalcWidget(new QRadioButton(sDrg[DRAD]), 0, 1, atype));
        connect(vec_btns.back(), SIGNAL(ClickServButton(QString)), SLOT(ProcessClickDRG(QString)));

        vec_btns.push_back(new QCalcWidget(new QRadioButton(sDrg[DGRAD]), 0, 2, atype));
        connect(vec_btns.back(), SIGNAL(ClickServButton(QString)), SLOT(ProcessClickDRG(QString)));
        return;
    case FUNCMODES:
        vec_btns.push_back(new QCalcWidget(new QCheckBox(sModes[INV]), 0, 0, atype));
        connect(vec_btns.back(), SIGNAL(ClickServButton(QString)), SLOT(ProcessClickFuncModes(QString)));

        vec_btns.push_back(new QCalcWidget(new QCheckBox(sModes[HYP]), 0, 0, atype));
        connect(vec_btns.back(), SIGNAL(ClickServButton(QString)), SLOT(ProcessClickFuncModes(QString)));
        return;
    case SERVBUTTONS:
        vec_btns.push_back(new QCalcWidget(new QPushButton(), 0, 0, atype, sServ[BACKSPACE]));
        connect(vec_btns.back(), SIGNAL(ClickButton(QString)), SLOT(ProcessClickServ(QString)));

        vec_btns.push_back(new QCalcWidget(new QPushButton(), 0, 0, atype, sServ[CE]));
        connect(vec_btns.back(), SIGNAL(ClickButton(QString)), SLOT(ProcessClickServ(QString)));

        vec_btns.push_back(new QCalcWidget(new QPushButton(), 0, 0, atype, "C", sServ[ESC]));
        connect(vec_btns.back(), SIGNAL(ClickButton(QString)), SLOT(ProcessClickServ(QString)));
        return;
    default:
        return;
    }

    for(unsigned i = 0; i < max_i; ++i)
        for(unsigned j = 0; j < max_j; ++j)
        {
            index = max_j * i + j;            
            cw = new QCalcWidget(new QPushButton(), i, j, atype, s[index], s_v[index]);
            cw->SetTextColor(color);

            if(atype == MEM)
                connect(cw, SIGNAL(ClickButton(QString)), SLOT(ProcessClickMem(QString)));
            else
                connect(cw, SIGNAL(ClickButton(QString)), SLOT(ProcessClick(QString)));

            if(atype == FUNC)
            {
                cw->SetAltTexts(svAltFunc[index], sAltFunc[index]);
                cw->hyp = sHypInv[index] == "h";
                cw->inv = sHypInv[index] == "i";
            }
            if(atype == OP)
            {
                cw->SetNopable(bOpNops[index]);
                cw->SetReplaceValue(sOpReps[index]);
            }


            vec_btns.push_back(cw);
        }
}



void MainWidget::ResizeWidgets(unsigned w, unsigned h, pnl atype)
{
    foreach (QCalcWidget* widget, vec_btns) { if(widget->GetType() == atype) widget->SetSize(w, h);}
}


void MainWidget::SetSizeOfWidgets(unsigned button_w, unsigned button_h)
{
    int i_left, i_top, i_right, i_bottom;
    unsigned w_bord, h_bord;
    unsigned func_button_h = GetHFButton(button_h);

    i_left = i_right = i_top = i_bottom = 0;
    wDigits->getContentsMargins(&i_left, &i_top, &i_right, &i_bottom);
    w_bord = i_left + i_right;
    h_bord = i_top + i_bottom;

    this->getContentsMargins(&i_left, &i_top, &i_right, &i_bottom);

    wDigits->setFixedSize(button_w * 3 + spacing * 4 + w_bord, button_h * 4 + spacing * 5 + h_bord);

    switch(viewCalc)
    {
    case ORIGINAL:
        wOps->setFixedSize(button_w * 3 + spacing * 4 + w_bord, button_h * 4 + spacing * 5 + h_bord);
        wAbc->setFixedSize(button_w * 3 + spacing * 4 + w_bord, button_h * 2 + spacing * 3 + h_bord);
        wMem->setFixedSize(wAbc->width(), wOps->height() - wAbc->height() - spacing);
        wBottom->setFixedSize(wDigits->width() + wOps->width() + wMem->width() + spacing * 2, wDigits->height());

        wCentral->setFixedSize(wBottom->width(), func_button_h * 3 + spacing * 4 + h_bord);
        button_func_w = (wCentral->width() - spacing * 8) / 7;

        wDisplay->setFixedSize(wBottom->width(), button_h + h_bord);
        //Display->setFixedSize(wDisplay->width() - w_bord, wDisplay->height() - h_bord);

        wScale->setFixedSize(button_func_w * 4 + spacing * 4, GetHFButton(button_h) + h_bord);
        wDRG->setFixedSize(wBottom->width() - wScale->width() - spacing, wScale->height());


        wFuncModes->setFixedSize(button_func_w * 2 + spacing * 3, wScale->height());
        ResizeWidgets(button_w, wFuncModes->height(), SERVBUTTONS);
        wMode->setFixedSize(wBottom->width(), wScale->height() + wFuncModes->height() + spacing);

        this->setFixedSize(wBottom->width() + i_left + i_right,
                           wBottom->height() + wCentral->height() + wMode->height() +
                           wDisplay->height() +  spacing * 4 + MenuBar->height() + i_top + i_bottom);
        break;
    case SIMPLE:
        wOps->setFixedSize(button_w * 2 + spacing * 3 + w_bord, button_h * 4 + spacing * 5 + h_bord);
        wBottom->setFixedSize(wDigits->width() + wOps->width() + spacing, wDigits->height());
        wDisplay->setFixedSize(wBottom->width(), button_h + h_bord);
        wMode->setFixedSize(wBottom->width(), GetHFButton(button_h) + h_bord);
        ResizeWidgets(button_w, wMode->height(), SERVBUTTONS);

        this->setFixedSize(wBottom->width() + i_left + i_right,
                           wBottom->height() + wMode->height() +
                           wDisplay->height() +  spacing * 3 + MenuBar->height() + i_top + i_bottom);

        break;
    case PROGRAMMABLE:
        //
        break;
    default:
        return;
    }
}


void MainWidget::ResizeAll(unsigned new_button_w, unsigned new_button_h)
{
    button_w = new_button_w;
    button_h = new_button_h;

    int logical_w = (QPaintDevice::logicalDpiX() * button_w) / DEFAULT_DPI;
    int logical_h = (QPaintDevice::logicalDpiY() * button_h) / DEFAULT_DPI;

    ResizeWidgets(logical_w, logical_h, DIG);
    ResizeWidgets(logical_w, logical_h, OP);
    ResizeWidgets(logical_w, logical_h, MEM);
    ResizeWidgets(logical_w, logical_h, ABC);

    SetSizeOfWidgets(logical_w, logical_h);

    ResizeWidgets(button_func_w, GetHFButton(logical_h), FUNC);
    ResizeWidgets(button_func_w + 2, GetHFButton(logical_h), SCALE);
    ResizeWidgets(button_func_w + 2, GetHFButton(logical_h), DRG);
    ResizeWidgets(button_func_w + 2, GetHFButton(logical_h), FUNCMODES);

    posMousePress.setX(logical_w / 2);
    posMousePress.setY(logical_h / 3);
    ReCreateMouseEvents();
}

bool MainWidget::AddToken(const QString& stok)
{
    bool res;
#ifdef _QT4
    QString s = stok;
    res = parser->AddToken(&s);
#else
    std::string stdstring = stok.toStdString();
    res = parser->AddToken(&stdstring);
#endif
    if(!res)
        Alert();
    return res;
}


QString MainWidget::NumberToString(double n, int precision)
{
#ifdef _QT4
    return parser->DoubleToString(n, precision);
#else
    return QString::fromStdString(parser->DoubleToString(n, precision));
#endif
}

void MainWidget::ProcessClick(const QString& sButtonValue)
{
    if(sButtonValue == "=")
    {
        if(parser->Run())
            UpdateDisplay(RES);
        else
            UpdateDisplay(ERRS);
    }
    else
    {
        QString s = sButtonValue;
        if(s == "^2")                            //Для перевода степени в нужную систему счисления
            s = "^" + NumberToString(2);
        if(s == "^3")
            s = "^" + NumberToString(3);

        AddToken(s);
        UpdateDisplay();
    }
}


void MainWidget::ProcessClickMem(const QString& sButtonValue)
{
    QString stok = sButtonValue;

    if(stok == sMem[0])
    {
        if(parser->Run())
        {
            inMemory = parser->GetResult();
            lblMem->setText(NumberToString(inMemory, 6));
        }
        else
            UpdateDisplay(ERRS);

    }
    if(stok == sMem[1])
    {
        if(lblMem->text().isEmpty())
            return;

        QString sMemory = NumberToString(fabs(inMemory), 16);
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
    }

}


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


void MainWidget::ProcessClickServ(const QString& sButtonValue)
{
    if(sButtonValue == "Back")
        parser->ToBack();
    if(sButtonValue == "CE")
        parser->ToBack(false);
    if(sButtonValue == "ESC")
        parser->SetParams(NULL, parser->Scale(), parser->DRG());

    UpdateDisplay();
}


void MainWidget::ProcessClickDRG(const QString& sButtonValue)
{

    int drg_mode = 0;

    if(sButtonValue == "Deg")
        drg_mode = RDEG;
    if(sButtonValue == "Rad")
        drg_mode = RRAD;
    if(sButtonValue == "Grad")
        drg_mode = RGRAD;
    parser->SetDRG(drg_mode);

    UpdateDisplay();
}


void MainWidget::ProcessClickFuncModes(const QString& sModeValue)
{
    QCalcWidget* w = FindButtonByValue(sModeValue);
    if(!w) return;

    stFuncMode FM = {sModeValue, ((QCheckBox*)((QCalcWidget*)w->widget))->isChecked()};
    std::for_each(vec_btns.begin(), vec_btns.end(), bind2nd(setHypInv(), FM));
}


void MainWidget::SetMode(SMODES mode, bool on)
{
    std::vector<QCalcWidget*> btns_fm;
    btns_fm.resize(std::count_if(vec_btns.begin(), vec_btns.end(), bind2nd(checkBtnType(), FUNCMODES)));
    copy_if(vec_btns.begin(), vec_btns.end(), btns_fm.begin(), bind2nd(checkBtnType(), FUNCMODES));

    if(mode < INV || mode > HYP) return;
    QCheckBox *cb = ((QCheckBox*)((QCalcWidget*)btns_fm.at(mode)->widget));
    if(cb->isChecked() != on)
        cb->click();        
}


QString MainWidget::GetExpression(bool bHtml)
{
#ifdef _QT4
    return parser->GetExpression("", bHtml);
#else
    return QString::fromStdString(parser->GetExpression("", bHtml));
#endif
}


QString MainWidget::GetErrors(void)
{
#ifdef _QT4
    return parser->listErrors();
#else
    return QString::fromUtf8(parser->listErrors().c_str());
#endif
}


void MainWidget::UpdateDisplay(ud how_update)
{
    QString expression;
    QString errors;

    expression = GetExpression(true);
    errors = GetErrors();

    Display->setText(expression);
    if(how_update == ERRS)
        wResult->setText(errors);
    else
        if(how_update == RES)
            wResult->setText("= " + NumberToString(parser->GetResult(), 16));
    else
            if(how_update == RESEMPTY)

                wResult->setText("");
    if(!lblMem->text().isEmpty())
        lblMem->setText(NumberToString(inMemory, 6));
}


void MainWidget::Alert(void)
{
    if(!bPasting)
        QApplication::beep();
}



QCalcWidget* MainWidget::FindButtonByValue(QString value)
{
    std::vector<QCalcWidget*>::iterator it = std::find_if(vec_btns.begin(), vec_btns.end(), bind2nd(buttonIsValue(), value));
    return *it;
}


void MainWidget::AssignKeyToButton(QString button_value, int key, int mod)
{
    QCalcWidget *calcwidget = NULL;
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

