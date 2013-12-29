#include "visualizationwidget.h"
#include <QPaintEvent>

//----------------------------------------------------------------------------//
VisualizationWidget::VisualizationWidget( QWidget *parent) 
	: QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
	setAutoFillBackground(false);
	brush_.setColor(Qt::white);
	brush_.setStyle(Qt::SolidPattern);
	fitVideo_ = false;
	cycles_ = 0;
}
//----------------------------------------------------------------------------//
void VisualizationWidget::reloadVideoFrame(const QImage& newFrame)
{		
	videoFrame_ = QPixmap::fromImage( newFrame);
	if (!videoFrame_.isNull())
	{
		if (cycles_ == 0)
		{
			sx_ = (qreal) width() / videoFrame_.width();
			sy_ = (qreal) height() / videoFrame_.height();
		}
		cycles_++;
		update();
	}
}
//----------------------------------------------------------------------------//
void VisualizationWidget::paintEvent(QPaintEvent *event)
{
	painter_.begin(this);	
	if (videoFrame_.isNull())
	{
		painter_.fillRect(event->rect(), brush_);
		painter_.drawText(event->rect(), Qt::AlignCenter, "No video");
	}
	else
	{
		if (fitVideo_) painter_.scale( sx_, sy_);
		painter_.drawPixmap(videoFrame_.rect(), videoFrame_, videoFrame_.rect());
	}
	painter_.end();
}
//----------------------------------------------------------------------------//
void VisualizationWidget::resizeEvent( QResizeEvent * /*event*/)
{
 	sx_ = (qreal) width() / videoFrame_.width();
 	sy_ = (qreal) height() / videoFrame_.height(); 	
}
//----------------------------------------------------------------------------//
