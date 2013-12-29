//----------------------------------------------------------------------------//
//                           VISUALIZATION WIDGET                             //
//----------------------------------------------------------------------------//
//  Author: Łukasz Korbel, e-mail: korbel85@gmail.com                         //
//  Licence: GPL                                                              //
//  Version: 3.00                                                             //
//----------------------------------------------------------------------------//
//  Version info:                                                             //
//  3.00 - removed rendering, only frame copying                              //
//  2.00 - version for FaceFinder2                              // 29.09 2009 // 
//----------------------------------------------------------------------------//

#ifndef VISUALIZATIONWIDGET_H
#define VISUALIZATIONWIDGET_H

//includes
//----------------------------------------------------------------------------//
#include <QGLWidget>
#include <QPainter>
#include <QBrush>
#include <QPixmap>

//Visualization class
//----------------------------------------------------------------------------//
class VisualizationWidget : public QGLWidget
{
Q_OBJECT
public:
	VisualizationWidget( QWidget *parent = 0);
	bool fitVideo() const;
	void resetCycles();
public slots:
	void fitVideo(bool on);
	void reloadVideoFrame(const QImage& newFrame);
protected:
	void paintEvent(QPaintEvent *event);
	void resizeEvent( QResizeEvent *event);
private:
	QPainter painter_;
	QBrush brush_;
	bool fitVideo_;
	QPixmap videoFrame_;
	qreal sx_, sy_;
	int cycles_;
};
//----------------------------------------------------------------------------//
inline void VisualizationWidget:: resetCycles()
{
	cycles_ = 0;
}
//----------------------------------------------------------------------------//
inline bool VisualizationWidget:: fitVideo() const
{
	return fitVideo_;
}
//----------------------------------------------------------------------------//
inline void VisualizationWidget:: fitVideo(bool on)
{
	fitVideo_ = on;
}
//----------------------------------------------------------------------------//

#endif
