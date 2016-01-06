#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Algorithm.h"
#include "Parameters.h"

#include <vector>
#include <map>
#include <cstdint>
#include <QMainWindow>
#include <QListWidget>
#include <QPixmap>
#include <QLabel>
#include <QHBoxLayout>
#include <QCheckBox>

class MainWindow : public QMainWindow
{
   Q_OBJECT

public:
   explicit MainWindow(Parameters &params, QWidget *parent = 0);
   virtual ~MainWindow();

   void addAlgorithm(const std::string &name, Algorithm *algorithm);

   QCheckBox * addCheckBox(const std::string &name);
   QLineEdit * addIntLineEdit(const std::string &name);
   QLineEdit * addDoubleLineEdit(const std::string &name);

private:
   QListWidget *thumbs;
   std::vector<QPixmap> images;
   QLabel *cur_img;
   QVBoxLayout *layoutr;
   uint32_t cur_idx;

   Parameters &params;
   std::map<std::string, Algorithm*> algorithms;

   void add_img(const QPixmap &pm);
   void update_thumb(uint32_t idx);
   void update_cur_img();
   void get_selected_images(std::vector<uint32_t> &imgs);
   void select_img(uint32_t idx);
   void run_algorithm(const std::string &name);

   // void addCheckbox(const std::string &name, bool &field);

private slots:
   void on_open_image();
   void on_image_change(QListWidgetItem *item);

   // void on_apply_filter();
   // void on_apply_stitching();
};

#endif
