#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "manova.h"
#include "anova.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->textBrowser_3->setText("Введите в белое поле данные для исcледования алгоритмом \n"
                               "Пример:  если вам необходимо исследовать на зависимость три группы \n"
                               "1 группа : 1, 2.5, 3;   2 группа : 4, 5.2;   3 группа : 6, 7, 8.3, 9 \n"
                               "Необходимо ввести в поле данные в следующем виде: \n"
                               "((1, 2.5, 3), (4, 5.2), (6, 7, 8.3, 9))");
    ui->textBrowser_4->setText("Введите в белое поле данные для исcледования алгоритмом \n"
                               "Пример:  если вам необходимо исследовать на зависимость три группы \n"
                               "где рассматриваются две случайные величины ν и η \n"
                               "1 группа по ν: 1, 2.5, 3;   2 группа по ν: 4, 5.2;   3 группа по ν: 6, 7, 8.3, 9 \n"
                               "1 группа по η: 11.8, 12, 13;   2 группа по η: 14, 15;   3 группа по η: 16, 17, 18, 19 \n"
                               "Необходимо ввести в поле данные в следующем виде: \n"
                               "(((1, 11.8), (2.5, 12), (3, 13)), \n((4, 14), (5.2, 15)),\n((6, 16), (7, 17), (8.3, 18), (9, 19)))");

    ui->lineEdit->setText("0.05");
    ui->lineEdit_2->setText("0.05");

}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::on_pushButton_clicked()
{
}


void MainWindow::on_pushButton_2_clicked()
{
    ui->textBrowser_2->clear();
    alpha = ui->lineEdit->text().toDouble();
    s = ui->textEdit->toPlainText().toStdString();
    s.erase(remove(s.begin(),s.end(),' '),s.end());//убираем пробелы
    //парсинг
    std::vector<std::vector<double>> v;
    size_t count_open = 0;
    std::string tmp = "";
    std::vector<double> v_tmp;
    for(size_t i = 0; i < s.size(); i++){
        if(count_open==0 && s[i]=='('){
            count_open++;
        }else if(count_open==1 && s[i]=='('){
            count_open++;
        }
        else if(count_open==2 && s[i]!=',' && s[i]!=')'){
            tmp+=s[i];
        }
        else if(count_open==2 && s[i]==','){
            v_tmp.push_back(std::stod(tmp));
            tmp="";
        }
        else if(count_open==2 && s[i]==')'){
            v_tmp.push_back(std::stod(tmp));
            tmp="";
            count_open--;
            v.push_back(v_tmp);
            v_tmp.resize(0);
        }
        else if(count_open==1 && s[i]==')'){
            break;
        }
    }


    //получаем результаты в файлых
    ANOVA a(v, alpha);
    a.analyse();

    //выводим результаты из файлов в окно
    std::ifstream anova_res("Anova_res.txt");
     while(getline(anova_res, tmp)){
         qs += QString::fromStdString(tmp+'\n');
     }

     ui->textBrowser_2->setText(qs);
     qs = "";
     s = "";
    anova_res.close();

}

void MainWindow::on_pushButton_3_clicked()
{
    alpha = ui->lineEdit_2->text().toDouble();
    s = ui->textEdit_2->toPlainText().toStdString();
    s.erase(remove(s.begin(),s.end(),' '),s.end());//убираем пробелы
    //парсинг
    std::vector<std::vector<std::vector<double>>> v;
    std::vector<std::vector<double>> vv;
    size_t count_open = 0;
    std::string tmp = "";
    std::vector<double> v_tmp;
    for(size_t i = 0; i < s.size(); i++){
        if(count_open<=2 && s[i]=='('){
            count_open++;
        }
        else if(count_open==3 && s[i]!=',' && s[i]!=')'){
            tmp+=s[i];
        }
        else if(count_open==3 && s[i]==','){
            v_tmp.push_back(std::stod(tmp));
            tmp="";
        }
        else if(count_open==3 && s[i]==')'){
            v_tmp.push_back(std::stod(tmp));
            tmp="";
            count_open--;
            vv.push_back(v_tmp);
            v_tmp.resize(0);
        }
        else if(count_open==2 && s[i]==')'){
            count_open--;
            v.push_back(vv);
            vv.resize(0);
        }
        else if(count_open==1 && s[i]==')'){
            break;
        }
    }


    //получаем результаты в файлых
    MANOVA m(v, alpha);
    m.analyse();

    //выводим результаты из файлов в окно
    std::ifstream manova_res("Manova_res.txt");
     while(getline(manova_res, tmp)){
         qs += QString::fromStdString(tmp+'\n');
     }
     ui->textBrowser->clear();
     ui->textBrowser->setText(qs);
     qs = "";
     s = "";
     manova_res.close();
}
