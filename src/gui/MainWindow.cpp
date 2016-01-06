#include "MainWindow.h"
#include "Callback.h"
#include "Image.h"

#include <functional>

#include <QPushButton>
#include <QScrollArea>
#include <QFileDialog>
#include <QLineEdit>

#define THUMB_W 100
#define THUMB_H 100
#define THUMB_MARGIN 10
#define CUR_W 640
#define CUR_H 480
#define OPTBAR_W 128

MainWindow::MainWindow(Parameters &p, QWidget *parent)
 : QMainWindow(parent)
 , params(p)
{
   setWindowTitle("Praktikum 3");

   // buttons
   QPushButton *btn_open = new QPushButton("Open image");

   // current image
   cur_img = new QLabel;
   cur_img->setFixedSize(CUR_W, CUR_H);
   cur_img->setAlignment(Qt::AlignCenter);

   // layouts
   QHBoxLayout *layout = new QHBoxLayout;
   QVBoxLayout *layoutl = new QVBoxLayout;
   layoutr = new QVBoxLayout;
   layoutl->setAlignment(Qt::AlignTop);
   layoutr->setAlignment(Qt::AlignTop);
   layout->addLayout(layoutl);
   layout->addWidget(cur_img);
   layout->addLayout(layoutr);

   // thumbnails
   QScrollArea *thumbsscrollbar = new QScrollArea;

   thumbs = new QListWidget();
   thumbsscrollbar->setMaximumWidth(THUMB_W + THUMB_MARGIN * 4);
   btn_open->setMaximumWidth(THUMB_W + THUMB_MARGIN * 4);
   thumbs->setSelectionMode(QAbstractItemView::ExtendedSelection);
   thumbsscrollbar->setWidgetResizable(true);
   thumbsscrollbar->setWidget(thumbs);

   // add widgets
   layoutl->addWidget(thumbsscrollbar);
   layoutl->addWidget(btn_open);

   QWidget *window = new QWidget();
   window->setLayout(layout);
   setCentralWidget(window);

   // connect signals
   connect(btn_open, SIGNAL(clicked()), this, SLOT(on_open_image()));
   connect(thumbs, SIGNAL(itemClicked(QListWidgetItem*)),
           this, SLOT(on_image_change(QListWidgetItem*)));


}

MainWindow::~MainWindow() {}

void MainWindow::get_selected_images(std::vector<uint32_t> &imgs)
{
   QList<QListWidgetItem*> items = thumbs->selectedItems();
   for (QList<QListWidgetItem*>::iterator it = items.begin();
        it != items.end(); ++it) {
      imgs.push_back(thumbs->row(*it));
   }
}

void MainWindow::on_open_image()
{
   QString filters("Images (*.png *.jpg *.jpeg *.tiff *.ppm *.pgm);;All files (*.*)");
   QString default_filter("Images (*.png *.jpg *.jpeg *.tiff *.ppm *.pgm)");

   QStringList imgfiles = QFileDialog::getOpenFileNames(
      0, "Open image", QDir::homePath(), filters, &default_filter);

   for (QStringList::iterator it = imgfiles.begin();
       it < imgfiles.end(); ++it) {
      add_img(QPixmap(*it));
   }
}

void MainWindow::on_image_change(QListWidgetItem *item)
{
   cur_idx = thumbs->row(item);
   update_cur_img();
}

void MainWindow::update_cur_img()
{
   cur_img->setPixmap(images[cur_idx].scaled(CUR_W, CUR_H,
                      Qt::KeepAspectRatio, Qt::FastTransformation));
}

static void convert(const QImage &src, Image &dst)
{
   // get dimensions
   const int M = src.height();
   const int N = src.width();

   dst = Image(M,N);

   // traverse pixels
   for (int y = 0; y < M; ++y) {
      for (int x = 0; x < N; ++x) {
         QColor color( src.pixel(x,y) );
         const double r = (double) color.red();
         const double g = (double) color.green();
         const double b = (double) color.blue();
         dst[y][x] = Pixel(r,g,b);
      }
   }
}

static void convert(const Image &src, QImage &dst)
{
   // get dimensions
   const int M = src.height();
   const int N = src.width();

   QSize size(N,M);
   dst = QImage(size, QImage::Format_RGB32);

   // traverse pixels
   for (int y = 0; y < M; ++y) {
      for (int x = 0; x < N; ++x) {
         const Pixel &pixel = src[y][x];
         const int r = (int) pixel.r;
         const int g = (int) pixel.g;
         const int b = (int) pixel.b;
         dst.setPixel(x, y, qRgb(r,g,b));
      }
   }
}


void MainWindow::run_algorithm(const std::string &name)
{
   Algorithm *algo = algorithms[name];

   std::vector<uint32_t> selimgs;
   get_selected_images(selimgs);
   uint32_t idx = images.size();

   Image dst;
   Image src;

   QImage src_qt;
   QImage dst_qt;

   for (uint32_t idx : selimgs) {
       QPixmap &pm = images[idx];

       // convert QImage to Image
       src_qt = pm.toImage().convertToFormat(QImage::Format_RGB32);
       convert(src_qt,src);

       // run the algorithm
       algo->process(params,src,dst);

       // convert back
       convert(dst,dst_qt);


       add_img(QPixmap::fromImage(dst_qt));
   }

   if (idx < images.size()) {
      select_img(idx);
   }
}


void MainWindow::addAlgorithm(const std::string &name, Algorithm *algorithm)
{
   // store algorithm
   algorithms[name] = algorithm;

   // add button
   QPushButton *btn_algorithm = new QPushButton(name.c_str());
   btn_algorithm->setMaximumWidth(OPTBAR_W);
   layoutr->addWidget(btn_algorithm);

   // create funciton callback for algorithm
   Callback::FunctionType f = std::bind(&MainWindow::run_algorithm, this, name);
   Callback *cb = makeCallback(f);

   // connect button and algorithm
   connect(btn_algorithm, SIGNAL(clicked()), cb, SLOT(execute()));
}


QCheckBox * MainWindow::addCheckBox(const std::string &name)
{
   QHBoxLayout *layout = new QHBoxLayout;

   QCheckBox *checkBox = new QCheckBox();
   checkBox->setMaximumWidth(OPTBAR_W);
   checkBox->setChecked(false);

   QLabel *label = new QLabel(name.c_str());
   layout->addWidget(label);
   layout->addWidget(checkBox);
   layout->setAlignment(Qt::AlignLeft);

   // layout->setMaximumWidth(OPTBAR_W);
   layoutr->addLayout(layout);

   return checkBox;
}

QLineEdit * MainWindow::addIntLineEdit(const std::string &name)
{
   QHBoxLayout *layout = new QHBoxLayout;

   QLineEdit *edit = new QLineEdit(name.c_str());
   edit->setValidator(new QIntValidator(this));

   QLabel *label = new QLabel(name.c_str());
   layout->addWidget(label);
   layout->addWidget(edit);
   layoutr->addLayout(layout);

   return edit;
}

QLineEdit * MainWindow::addDoubleLineEdit(const std::string &name)
{
   QHBoxLayout *layout = new QHBoxLayout;

   QLineEdit *edit = new QLineEdit(name.c_str());
   edit->setValidator(new QDoubleValidator(this));

   QLabel *label = new QLabel(name.c_str());
   layout->addWidget(label);
   layout->addWidget(edit);
   layoutr->addLayout(layout);

   return edit;
}

void MainWindow::add_img(const QPixmap &pm)
{
   uint32_t idx = images.size();
   images.push_back(pm);

   // add thumbnail
   QLabel *img = new QLabel;
   img->setMargin(THUMB_MARGIN);
   img->setFixedSize(THUMB_W + THUMB_MARGIN * 2, THUMB_H + THUMB_MARGIN * 2);
   QListWidgetItem *item = new QListWidgetItem();
   item->setSizeHint(QSize(THUMB_W + THUMB_MARGIN * 2, THUMB_H + THUMB_MARGIN * 2));
   thumbs->addItem(item);
   thumbs->setItemWidget(item, img);

   if (idx == 0)
      select_img(0);

   update_thumb(idx);
}

void MainWindow::select_img(uint32_t idx)
{
   thumbs->clearSelection();
   thumbs->setCurrentItem(thumbs->item(idx));
   on_image_change(thumbs->item(idx));
}

void MainWindow::update_thumb(uint32_t idx)
{
   ((QLabel*)thumbs->itemWidget(thumbs->item(idx)))->setPixmap(
      images[idx].scaled(THUMB_W, THUMB_H, Qt::KeepAspectRatio, Qt::FastTransformation));
}

