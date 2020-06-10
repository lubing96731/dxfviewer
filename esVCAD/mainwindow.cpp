#include "mainwindow.h"

#include "entities/coordxy.h"
#include "esvcaditem.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QGraphicsView>
#include <QMessageBox>
#include <QModelIndex>
#include <QStandardItemModel>
#include <QStringListModel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  ui->pushButtonMoveDown->setVisible(false);
  ui->pushButtonMoveUp->setVisible(false);
  ui->pushButtonMoveLeft->setVisible(false);
  ui->pushButtonMoveRight->setVisible(false);
  ui->pushButtonZoomIn->setVisible(false);
  ui->pushButtonZoomOut->setVisible(false);
  ui->pushButtonDxfParse->setVisible(false);
  ui->centralWidget->setMouseTracking(true);
  ui->m_graphicsFrame->setVisible(false);
  this->setMouseTracking(true);

  m_scene = new QGraphicsScene;
  // m_scene->setSceneRect(0,0,1024,1024);
  m_scene->setBackgroundBrush(QBrush(Qt::black));
  ui->graphicsView->setAlignment(
      Qt::AlignCenter); //(Qt::AlignTop|Qt::AlignLeft);
  ui->graphicsView->setScene(m_scene);
  ui->graphicsView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  ui->graphicsView->show();
}

MainWindow::~MainWindow() {
  delete ui;
}

void MainWindow::ShowEntities(std::vector<Block *> &blocks) {
  ui->listWidgetEntities->clear();
  PainterAttributes painterAttributes;
  QPen pen;
  pen.setColor(Qt::yellow);
  pen.setWidth(1);
  painterAttributes.m_pen = pen;
  int m                   = 0;
  for (size_t i = 0; i < blocks.size(); ++i) {
    Block *block = blocks[i];
    if (!block->IsEmpty()) {
      for (size_t k = 0; k < block->GetElementSize(); ++k) {
        block->ElementAt(k)->SetPainterAttributes(painterAttributes);
        QString s = QString::fromStdString(block->ElementAt(k)->ToString());
        //+"___"+QString::fromStdString(block->ElementAt(k)->GetId())
        ui->listWidgetEntities->addItem(
            s + QString::fromStdString(block->ElementAt(k)->GetId()));
        m++;
      }
    }
  }
  ui->listWidgetEntities->addItem(QString::number(m));
}
void MainWindow::ParseDxf(const string &fileName) {
  int width  = ui->graphicsView->width();
  int height = ui->graphicsView->height();
  m_scene->clear();
  EsvCadItem *esvCadItem = new EsvCadItem(width, height);
  esvCadItem->ParseDxf(QString::fromStdString(fileName));
  m_scene->addItem(esvCadItem);
  std::vector<Block *> &blocks = esvCadItem->GetBlocks();
  ShowEntities(blocks);
}

void MainWindow::on_pushButtonSelFile_clicked() {
  QFileDialog *fd = new QFileDialog(this);
  fd->setWindowTitle("请选择dxf文件");
  fd->setNameFilter(tr("dxf(*.dxf)"));
  if (fd->exec() == QFileDialog::Accepted) // ok
  {
    QStringList fileNameLists = fd->selectedFiles();
    if (fileNameLists.size() > 0) {
      m_sFileName = fileNameLists.at(0);
      on_pushButtonDxfParse_clicked();
    } else {
      QMessageBox::information(nullptr, "dxf选择错误", "请选择dxf文件",
          QMessageBox::Yes, QMessageBox::Yes);
    }
  }
}

void MainWindow::on_pushButtonDxfParse_clicked() {
  if (m_sFileName != "") { ParseDxf(m_sFileName.toStdString()); }
}

void MainWindow::on_pushButtonMoveRight_clicked() {
  int dx                           = 20;
  std::vector<Block *> &blocks     = m_esvCad.GetBlocks();
  vector<Block *>::size_type count = blocks.size();
  for (size_t i = 0; i < count; ++i) { blocks[i]->Transfer(dx, 0.0, 0.0); }
  ui->m_graphicsFrame->PaintEntities(blocks);
}

void MainWindow::on_pushButtonMoveLeft_clicked() {
  int dx                           = -20;
  std::vector<Block *> &blocks     = m_esvCad.GetBlocks();
  vector<Block *>::size_type count = blocks.size();
  for (size_t i = 0; i < count; ++i) { blocks[i]->Transfer(dx, 0.0, 0.0); }
  ui->m_graphicsFrame->PaintEntities(blocks);
}

void MainWindow::on_pushButtonMoveUp_clicked() {
  int dy                           = -20;
  std::vector<Block *> &blocks     = m_esvCad.GetBlocks();
  vector<Block *>::size_type count = blocks.size();
  for (size_t i = 0; i < count; ++i) { blocks[i]->Transfer(0.0, dy, 0.0); }
  ui->m_graphicsFrame->PaintEntities(blocks);
}

void MainWindow::on_pushButtonMoveDown_clicked() {
  int dy                           = 20;
  std::vector<Block *> &blocks     = m_esvCad.GetBlocks();
  vector<Block *>::size_type count = blocks.size();
  for (size_t i = 0; i < count; ++i) { blocks[i]->Transfer(0.0, dy, 0.0); }
  ui->m_graphicsFrame->PaintEntities(blocks);
}

void MainWindow::on_pushButtonZoomOut_clicked() {
  double scale                     = 2;
  std::vector<Block *> &blocks     = m_esvCad.GetBlocks();
  vector<Block *>::size_type count = blocks.size();
  for (size_t i = 0; i < count; ++i) { blocks[i]->Scale(scale); }
  ui->m_graphicsFrame->PaintEntities(blocks);
}

void MainWindow::on_pushButtonZoomIn_clicked() {
  double scale                     = 0.5;
  std::vector<Block *> &blocks     = m_esvCad.GetBlocks();
  vector<Block *>::size_type count = blocks.size();
  for (size_t i = 0; i < count; ++i) { blocks[i]->Scale(scale); }
  ui->m_graphicsFrame->PaintEntities(blocks);
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event) {
  //进行坐标转换
  if (event->buttons() == Qt::MiddleButton) {
    int width        = ui->m_graphicsFrame->width();
    int height       = ui->m_graphicsFrame->height();
    CoordXY &coordXY = m_esvCad.GetCoordXY();
    double xScale    = coordXY.GetWidth() / width;
    double yScale    = coordXY.GetHeihgt() / height;
    double scale     = std::max(xScale, yScale);
    Point leftTop(coordXY.GetLeftTop());
    double params[5] = {
        scale, (double)height, leftTop.GetX(), leftTop.GetY(), (double)width};
    coordXY.Transform(params, 5);
    std::vector<Block *> &blocks     = m_esvCad.GetBlocks();
    std::vector<Layer *> &layers     = m_esvCad.GetLayers();
    vector<Block *>::size_type count = blocks.size();
    for (size_t i = 0; i < count; ++i) {
      Block *block = blocks[i];
      block->Transform(layers, params, 5);
    }
    ui->m_graphicsFrame->PaintEntities(blocks);
  }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
  std::vector<Block *> &blocks = m_esvCad.GetBlocks();
  if (event->buttons() == Qt::MiddleButton) {
    m_dragEndPoint = event->pos();
    int dx         = m_dragEndPoint.x() - m_dragBeginPoint.x();
    int dy         = m_dragEndPoint.y() - m_dragBeginPoint.y();
    m_esvCad.GetCoordXY().Transfer(dx, dy, 0.0);
    std::vector<Block *> &blocks     = m_esvCad.GetBlocks();
    vector<Block *>::size_type count = blocks.size();
    for (size_t i = 0; i < count; ++i) { blocks[i]->Transfer(dx, dy, 0.0); }
    m_dragBeginPoint = m_dragEndPoint;
  } else if (event->buttons() == Qt::NoButton) {
    ui->m_graphicsFrame->GetPickedEntity(blocks, event->pos());
  }
  ui->m_graphicsFrame->PaintEntities(blocks);
}
void MainWindow::mousePressEvent(QMouseEvent *event) {
  if (event->buttons() == Qt::MiddleButton) {
    m_dragBeginPoint = event->pos();
  } else if (event->buttons() == Qt::LeftButton) {
    m_scaleBasePoint = event->pos();
  }
}
void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
}
void MainWindow::wheelEvent(QWheelEvent *event) {
  //以鼠标当前位置为基点进行比例变换
  m_scaleBasePoint = event->pos();
  double scale     = 0.75;
  if (event->delta() > 0) {
    scale = 1.25;
  } else {
    scale = 0.75;
  }
  std::vector<Block *> &blocks     = m_esvCad.GetBlocks();
  vector<Block *>::size_type count = blocks.size();
  for (size_t i = 0; i < count; ++i) {
    blocks[i]->Transfer(-m_scaleBasePoint.x(), -m_scaleBasePoint.y(), 0.0);
    blocks[i]->Scale(scale);
    blocks[i]->Transfer(m_scaleBasePoint.x(), m_scaleBasePoint.y(), 0.0);
  }
  ui->m_graphicsFrame->PaintEntities(blocks);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
  //如果为文件，则支持拖放
  if (event->mimeData()->hasFormat("text/uri-list"))
    event->acceptProposedAction();
}
void MainWindow::dropEvent(QDropEvent *event) {
  //注意：这里如果有多文件存在，意思是用户一下子拖动了多个文件，而不是拖动一个目录
  //如果想读取整个目录，则在不同的操作平台下，自己编写函数实现读取整个目录文件名
  QList<QUrl> urls = event->mimeData()->urls();
  if (urls.isEmpty()) return;
  m_sFileName = urls.first().toLocalFile();
  if (m_sFileName != "") { ParseDxf(m_sFileName.toStdString()); }
}

void MainWindow::on_listWidgetEntities_clicked(const QModelIndex &index) {
  Q_UNUSED(index)
  int currenRow = ui->listWidgetEntities->currentRow();
  if (currenRow > 0) {}
}

void MainWindow::on_pushButtonClearSel_clicked() {
  for (int i = 0; i < m_scene->items().size(); ++i) {
    QGraphicsItem *item = m_scene->items().at(i);
    if (item->type() == QGraphicsItem::UserType + CAD) {
      EsvCadItem *cadItem = (EsvCadItem *)item;
      cadItem->HideSelEntities();
    }
  }
}

void MainWindow::on_listWidgetEntities_currentItemChanged(
    QListWidgetItem *current, QListWidgetItem *previous) {
}
