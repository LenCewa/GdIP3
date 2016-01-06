#include "gui/MainWindow.h"

#include "algorithms/Algorithm.h"

#include "algorithms/Greenify.h"

#include "algorithms/Richardson_Lucy_Algo.h"
#include "algorithms/Cartoonize.h"

#include "algorithms/LucyOMP.h"
#include "algorithms/CartoonizeOMP.h"

#include "algorithms/Parameters.h"

#include <memory>

#include <QApplication>
#include <QLineEdit>
#include <QLabel>


int main(int argc, char **argv)
{
   QApplication app(argc, argv);

   // structure containing parameters you can set
   Parameters params;

   // Create main window
   MainWindow window(params);

   // instantiate algorithms
   std::shared_ptr<Algorithm> greenify(new Greenify);

   std::shared_ptr<Algorithm> lucy(new Richardson_Lucy_Algo);
   std::shared_ptr<Algorithm> cartoonize(new Cartoonize);

   std::shared_ptr<Algorithm> lucyOMP(new LucyOMP);
   std::shared_ptr<Algorithm> cartoonizeOMP(new CartoonizeOMP);
   //------------------------------------
   // YOU SHOULD ADD SOMETHING HERE..
   //------------------------------------

   // add algorithms to gui
   window.addAlgorithm("Greenify", greenify.get());
   
   //------------------------------------
   // YOU SHOULD ADD SOMETHING HERE..
   //------------------------------------
   window.addAlgorithm("Lucy", lucy.get());
   window.addAlgorithm("Cartoonize", cartoonize.get());

   window.addAlgorithm("LucyOMP", lucyOMP.get());
   window.addAlgorithm("CartoonizeOMP", cartoonizeOMP.get());


   // Example code for adding some parameters to the GUI

   // add param 1
   QCheckBox *checkBox_params1 = window.addCheckBox("Param1:");
   checkBox_params1->setChecked(params.param1);
   MainWindow::connect(checkBox_params1, SIGNAL(toggled(bool)), &params, SLOT(setParam1(bool)));

   // add param 2
   QLineEdit *lineEdit_params2 = window.addIntLineEdit("Param2:");
   lineEdit_params2->setText(QString::number(params.param2));
   MainWindow::connect(lineEdit_params2, SIGNAL(textChanged(const QString&)), &params, SLOT(setParam2(QString)));

   // add param 3
   QLineEdit *lineEdit_param3 = window.addDoubleLineEdit("Param3:");
   lineEdit_param3->setText(QString::number(params.param3));
   MainWindow::connect(lineEdit_param3, SIGNAL(textChanged(const QString&)), &params, SLOT(setParam3(QString)));



   //------------------------------------
   // YOU SHOULD ADD SOMETHING HERE..
   //------------------------------------
   // new parameters

   // add paramL for the amount of Lucy-iterations
   QLineEdit *lineEdit_paramLucy = window.addDoubleLineEdit("Iteration for Lucy:");
   lineEdit_paramLucy->setText(QString::number(params.paramL));
   MainWindow::connect(lineEdit_paramLucy, SIGNAL(textChanged(const QString&)), &params, SLOT(setParamL(QString)));

   // add kernelSize, dig_d, sig_r, tau - for Cartoonizer
   QLineEdit *lineEdit_kernelCartoonize = window.addDoubleLineEdit("Kernel size Cartoonizer:");
   lineEdit_kernelCartoonize->setText(QString::number(params.kernelSize));
   MainWindow::connect(lineEdit_kernelCartoonize, SIGNAL(textChanged(const QString&)), &params, SLOT(setKernelSize(QString)));

   QLineEdit *lineEdit_sig_d = window.addDoubleLineEdit("Sig_d Cartoonizer:");
   lineEdit_sig_d->setText(QString::number(params.sig_d));
   MainWindow::connect(lineEdit_sig_d, SIGNAL(textChanged(const QString&)), &params, SLOT(setSig_d(QString)));

   QLineEdit *lineEdit_sig_r = window.addDoubleLineEdit("Sig_r Cartoonizer:");
   lineEdit_sig_r->setText(QString::number(params.sig_r));
   MainWindow::connect(lineEdit_sig_r, SIGNAL(textChanged(const QString&)), &params, SLOT(setSig_r(QString)));

   QLineEdit *lineEdit_tau = window.addDoubleLineEdit("Tau Cartoonizer:");
   lineEdit_tau->setText(QString::number(params.tau));
   MainWindow::connect(lineEdit_tau, SIGNAL(textChanged(const QString&)), &params, SLOT(setTau(QString)));

   // finally show the main window
   window.show();

   // Run main event loop
   return app.exec();
}
