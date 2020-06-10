#ifndef ESVGRAPHICSVIEW_H
#define ESVGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QWheelEvent>

class GraphicsView : public QGraphicsView {
 public:
  GraphicsView(QWidget *parent = 0);
  ~GraphicsView();

 public:
  void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;
  void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
  void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

 private:
  QPoint m_dragStartPoint;
  QPoint m_dragEndPoint;
  bool m_bDraging;
  double m_ratio;
};

#endif // ESVGRAPHICSVIEW_H
