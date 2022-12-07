#include "QDlgLogin.h"
#include "ui_QDlgLogin.h"
#include <QFile>
#include <QDebug>

QDlgLogin::QDlgLogin(int *userId, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QDlgLogin),
    m_userId(userId)
{
    ui->setupUi(this);

    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setAttribute(Qt::WA_InputMethodEnabled);

    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);

    db = QSqlDatabase::database("cellDataConn");
    qDebug() << db.databaseName() << db.tables();
    query = new QSqlQuery(db);

    query->setForwardOnly(true);
    loadStyleSheet(":/styles/loginStyle.qss");
    readSettings();
}

QDlgLogin::~QDlgLogin()
{
    delete ui;
}

void QDlgLogin::on_btnLogin_clicked()
{
    QString user = ui->editUserName->text().trimmed();
    QString pswd = ui->editPasswd->text().trimmed();
    QString encryptPSWD = encrypt(pswd);

    if (user.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("User name can't be void!"));
        return;
    }
    if (pswd.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("password can't be void"));
    }
    QString queryStr = QString("SELECT userId FROM userInfo WHERE name='%1' AND password='%2'").arg(user).arg(encryptPSWD);
    qDebug() << queryStr;
    if (query->exec(queryStr) && query->next()) {
        *m_userId = query->value(0).toInt();
        m_user = user;
        m_passwd = pswd;
        writeSettings();
        this->accept();
    } else {
        QMessageBox::warning(this, tr("Error"), tr("User name or password incorrect!"));
    }
}

void QDlgLogin::readSettings()
{
    QString organization = "SeekGene";
    QString appName = "Cell_Counter";
    QSettings settings(organization, appName);

    bool saved = settings.value("saved", false).toBool();
    m_user = settings.value("UserName", "user").toString();
    QString defaultPSWD = encrypt("12345");
    m_passwd = settings.value("PSWD", defaultPSWD).toString();

    if (saved) {
        ui->editUserName->setText(m_user);
        ui->editPasswd->setText(m_passwd);
    }
    ui->chkSave->setChecked(saved);
}

void QDlgLogin::writeSettings()
{
    QSettings settings("SeekGene", "Cell_Counter");
    settings.setValue("UserName", m_user);
    settings.setValue("PSWD", m_passwd);
    settings.setValue("saved", ui->chkSave->isChecked());
}

QString QDlgLogin::encrypt(const QString &str)
{
    QByteArray btArray;

    btArray.append(str.toLatin1());
    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(btArray);
    QByteArray resultArray = hash.result();

    QString md5 = resultArray.toHex();

    return md5;
}

void QDlgLogin::loadStyleSheet(const QString &styleSheetFile)
{
    QFile file(styleSheetFile);
    file.open(QFile::ReadOnly);
    if (file.isOpen()) {
        QString styleSheet = this->styleSheet();
        styleSheet += QLatin1String(file.readAll());
        this->setStyleSheet(styleSheet);
        file.close();
    } else {
        qDebug() << "Login: Open Style Sheet File Failed!";
    }
}


