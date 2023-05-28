#include "mainwindow.h"
#include "ui_mainwindow.h"

// ------------------- Main Window -------------------
MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    date.setDateLabel(ui->date_label);
    date.print();

    topPanelObj.setExit(ui->actionExit);
    topPanelObj.setTop(ui->actionAlways_on_top);
    topPanelObj.setReset(ui->actionReset_to_default);

    manager.setAdd(ui->Add);
    manager.setRemove(ui->Remove);
    manager.setEdit(ui->Edit);
    manager.setThingsList(ui->things_list);
    manager.setDone(ui->doneTask);
    manager.setExpected(ui->expectedTask);
    manager.setFailed(ui->failedTask);
    manager.setNext(ui->nextDay);
    manager.setPrevious(ui->previousDay);

    manager.makeConections(date);
    trayObj.makeConections(this);
    topPanelObj.makeConections(this);


    customize_list_font("Constantia", 17, 60);
    this->setWindowTitle("ToDoing");
    this->setWindowIcon(QIcon(":/icon.png"));

    manager.getFile()->getTasksFromFile(&manager, date);
}

MainWindow::~MainWindow() {

    manager.getFile()->putTasksIntoFile(&manager, date);

    delete ui;
}

// Дії при закритті вікна
void MainWindow::closeEvent(QCloseEvent* event)
{
    event->ignore();
    this->hide();
}

void MainWindow::customize_list_font(QString font_name, int size, int block_height) {
    QFont font(font_name, size);
    ui->things_list->setFont(font);
    ui->things_list->setStyleSheet(QString("QListWidget::item { height:%1px; }").arg(block_height));
}




// ------------------- File Data -------------------
FileData::FileData()
{
    basePath = QCoreApplication::applicationDirPath() + "\\tasks_base\\";
    task_state.insert("flag_done", '2');
    task_state.insert("", '1');
    task_state.insert("flag_failed", '0');

    picture.insert('2', "://done.png");
    picture.insert('1', "://expected.png");
    picture.insert('0', "://failed.png");

    flag.insert('2', "flag_done");
    flag.insert('1', "");
    flag.insert('0', "flag_failed");
}

void FileData::getTasksFromFile(WidgetManager* manager, Date& date)
{
    QFile file(this->basePath + date.getCurrentDay() + ".txt");
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);

        while(!in.atEnd()) {
            QString line = in.readLine();
            const QChar state = line.at(0);

            QListWidgetItem* item = new QListWidgetItem(line.mid(1), manager->getThingsList());
            manager->getThingsList()->addItem(item);
            item->setFlags(item->flags() | Qt::ItemIsEditable);

            QIcon icon(picture[state]);
            item->setIcon(icon);

            item->setData(Qt::UserRole, flag[state]);
        }
    }

    file.close();
}

void FileData::putTasksIntoFile(WidgetManager* manager, Date& date)
{
    QDir directory(QCoreApplication::applicationDirPath());
    QString folderName = "tasks_base";
    if (!directory.exists(folderName)) {
        directory.mkpath(folderName);
    }

    QFile file(basePath + date.getCurrentDay() + ".txt");
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::information(0, "Writing error", file.errorString());
    }

    QTextStream out(&file);
    for (int i = 0; i < manager->getThingsList()->count(); ++i) {
        QString key = manager->getThingsList()->item(i)->data(Qt::UserRole).toString();

        out << task_state[key] << manager->getThingsList()->item(i)->text() << "\n";
    }

    file.close();
}




// ------------------- Widget Manager -------------------
WidgetManager::WidgetManager()
{
    this->add = nullptr;
    this->remove = nullptr;
    this->edit = nullptr;
    this->next = nullptr;
    this->previous = nullptr;

    this->file = new FileData;
    this->thingsList = nullptr;
}

WidgetManager::WidgetManager(QPushButton* addButton,
                  QPushButton* removeButton,
                  QPushButton* editButton,
                  QPushButton* nextButton,
                  QPushButton* prevButton,
                  FileData* file,
                  QListWidget* thingsList)
{
    this->add = addButton;
    this->remove = removeButton;
    this->edit = editButton;
    this->next = nextButton;
    this->previous = prevButton;

    this->file = file;
    this->thingsList = thingsList;
}

WidgetManager::~WidgetManager()
{
    delete add;
    delete remove;
    delete edit;
    delete done;
    delete expected;
    delete failed;
    delete next;
    delete previous;

    delete file;
    delete thingsList;
}


QListWidget* WidgetManager::getThingsList()
{
    return this->thingsList;
}

FileData* WidgetManager::getFile()
{
    return this->file;
}

void WidgetManager::setAdd(QPushButton* addButton)
{
    this->add = addButton;
}

void WidgetManager::setRemove(QPushButton* removeButton)
{
    this->remove = removeButton;
}

void WidgetManager::setEdit(QPushButton* editButton)
{
    this->edit = editButton;
}

void WidgetManager::setThingsList(QListWidget* thingsList)
{
    this->thingsList = thingsList;
}

void WidgetManager::setDone(QPushButton* doneButton)
{
    this->done = doneButton;
}

void WidgetManager::setExpected(QPushButton* expectedButton)
{
    this->expected = expectedButton;
}

void WidgetManager::setFailed(QPushButton* failedButton)
{
    this->failed = failedButton;
}

void WidgetManager::setNext(QPushButton* nextButton)
{
    this->next = nextButton;
}

void WidgetManager::setPrevious(QPushButton* previousButton)
{
    this->previous = previousButton;
}


void WidgetManager::on_doneTask_clicked() {      // Done
    QListWidgetItem* item = this->thingsList->currentItem();

    if(item){
        QIcon icon("://done.png");
        item->setIcon(icon);

       item->setData(Qt::UserRole, "flag_done");
    }
}

void WidgetManager::on_expectedTask_clicked() {  // Expected
    QListWidgetItem* item = this->thingsList->currentItem();

    if(item) {
        QIcon icon("://expected.png");
        item->setIcon(icon);

        item->setData(Qt::UserRole, "");
    }
}

void WidgetManager::on_failedTask_clicked() {    // Failed
    QListWidgetItem* item = this->thingsList->currentItem();

    if(item){
        QIcon icon("://failed.png");
        item->setIcon(icon);

        item->setData(Qt::UserRole, "flag_failed");
    }
}

void WidgetManager::on_Add_clicked(QListWidget* list)  // Add
{
    QListWidgetItem* item = new QListWidgetItem(list);
    item->setFlags(item->flags() | Qt::ItemIsEditable);

    QIcon icon("://expected.png");
    item->setIcon(QIcon(icon));

    list->setCurrentItem(item);                  // встановлюємо активність на елемент
    list->edit(list->currentIndex()); // викликаємо метод edit для редагування елемента
    item->setSelected(true);                                // виділяємо елемент
}

void WidgetManager::on_Remove_clicked(QListWidget* list)  // Remove
{
    QListWidgetItem* item = list->currentItem();
    if (item) {
        delete item;
    }
}

void WidgetManager::on_Edit_clicked(QListWidget* list)  // Edit
{
    QListWidgetItem* item = list->currentItem();
    if (item) {
        list->editItem(item);
    }
}

// Видалення пустих рядків(коли вони обрані)
void WidgetManager::afterChanged(QListWidgetItem* tempItem)
{
    if(tempItem != nullptr && tempItem->isSelected() && tempItem->text().isEmpty()){
        delete tempItem;
    }
}

void WidgetManager::makeConections(Date& date)
{
    QObject::connect(this->add, &QPushButton::clicked, this, [this](){
        this->on_Add_clicked(this->thingsList);
    });

    QObject::connect(this->remove, &QPushButton::clicked, this, [this](){
        this->on_Remove_clicked(this->thingsList);
    });

    QObject::connect(this->edit, &QPushButton::clicked, this, [this](){
        this->on_Edit_clicked(this->thingsList);
    });

    QObject::connect(this->done, &QPushButton::clicked, this, [this](){
        this->on_doneTask_clicked();
    });

    QObject::connect(this->expected, &QPushButton::clicked, this, [this](){
        this->on_expectedTask_clicked();
    });

    QObject::connect(this->failed, &QPushButton::clicked, this, [this](){
        this->on_failedTask_clicked();
    });

    QObject::connect(this->thingsList, &QListWidget::itemChanged, this, [this](){
        this->afterChanged(this->thingsList->currentItem());
    });

    QObject::connect(this->next, &QPushButton::clicked, this, [this, &date](){
        this->on_nextDay_clicked(date);
    });

    QObject::connect(this->previous, &QPushButton::clicked, this, [this, &date](){
        this->on_previousDay_clicked(date);
    });
}

void WidgetManager::on_previousDay_clicked(Date& date)
{
    this->file->putTasksIntoFile(this, date);
    this->thingsList->clear();

    (--date)->recount();
    date.print();
    this->file->getTasksFromFile(this, date);
}

void WidgetManager::on_nextDay_clicked(Date& date)
{
    this->file->putTasksIntoFile(this, date);
    this->thingsList->clear();

    (++date)->recount();
    date.print();
    this->file->getTasksFromFile(this, date);
}




// ------------------- Tray -------------------
void Tray::openFromTray(MainWindow* mw) {
    mw->show();
}

void Tray::quitFromTray() {
    QApplication::quit();
}

Tray::Tray()
{
    tray = new QSystemTrayIcon();
    tray->setIcon(QIcon("://icon.png"));
    tray->setVisible(true);

    trayMenu = new QMenu();
    trayOpen = trayMenu->addAction("Open");
    trayQuit = trayMenu->addAction("Quit");

    tray->setContextMenu(trayMenu);
}

Tray::~Tray()
{
    delete tray;
    delete trayOpen;
    delete trayQuit;
    delete trayMenu;
}

void Tray::makeConections(MainWindow* mw)
{
    QObject::connect(trayOpen, &QAction::triggered, this, [this, mw]() {
        this->openFromTray(mw);
    });
    QObject::connect(trayQuit, &QAction::triggered, this, &Tray::quitFromTray);
}




// ------------------- Top Panel -------------------

void TopPanel::menuAlways_on_top(MainWindow* mw) {
    mw->setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint); // закріплення вікна
    mw->show();
}

void TopPanel::menuReset_to_default(MainWindow* mw) {
    mw->setWindowFlags(Qt::Window);
    mw->show();
}


TopPanel::TopPanel()
{
    this->exit = nullptr;
    this->top = nullptr;
    this->reset = nullptr;
}

TopPanel::TopPanel(QAction* exitMenu, QAction* topMenu, QAction* resetMenu)
{
    this->exit = exitMenu;
    this->top = topMenu;
    this->reset = resetMenu;
}

TopPanel::~TopPanel()
{
    delete exit;
    delete top;
    delete reset;
}

void TopPanel::setExit(QAction* exitMenu)
{
    this->exit = exitMenu;
}

void TopPanel::setTop(QAction* topMenu)
{
    this->top = topMenu;
}

void TopPanel::setReset(QAction* resetMenu)
{
    this->reset = resetMenu;
}

void TopPanel::makeConections(MainWindow* mw)
{
    QObject::connect(this->exit, &QAction::triggered, this, [](){
         QApplication::quit();
    });

    QObject::connect(this->top, &QAction::triggered, this, [this, mw](){
        this->menuAlways_on_top(mw);
    });
    QObject::connect(this->reset, &QAction::triggered, this, [this, mw](){
        this->menuReset_to_default(mw);
    });
}


// ------------------- Date -------------------
Date::Date()
{
    this->daysCounter = 0;
    this->space = "----------------------";
    this->currentDay = this->get_date();
    this->dateLabel = nullptr;
}

Date::~Date()
{
    delete dateLabel;
}

void Date::print()
{
    this->dateLabel->setText(this->space + "  " + this->currentDay + "  " + this->space);
}

void Date::recount()
{
    this->currentDay = this->get_date();
}


QString Date::get_date() {
    return QDate::currentDate().addDays(this->daysCounter).toString("dd.MM.yyyy");
}

int Date::getDaysCounter()
{
    return this->daysCounter;
}

QString Date::getCurrentDay()
{
    return this->currentDay;
}

QLabel* Date::getDateLabel()
{
    return this->dateLabel;
}


void Date::setDaysCounter(int count)
{
    this->daysCounter = count;
}

void Date::setDateLabel(QLabel* dateLabel)
{
    this->dateLabel = dateLabel;
}


Date* Date::operator++()
{
    ++this->daysCounter;
    return this;
}

Date* Date::operator--()
{
    --this->daysCounter;
     return this;
}

Date* Date::operator++(int)
{
    Date temp = *this;
    ++this->daysCounter;
    return this;
}

Date* Date::operator--(int)
{
    Date temp = *this;
    --this->daysCounter;
    return this;
}

