#ifndef TEXTEFFECTS_H
#define TEXTEFFECTS_H
//

#include <QObject>
#include <QString>
#include <QQueue>
#include <QTimer>
//
class QPainter;
class QRect;

class TextEffects : public QObject
{
Q_OBJECT
public:
	TextEffects( QObject *parent);
	void loadText(const QString& text);
	void setShowTime(int showTime);
	void setPainter( QPainter* painter);
	void setRect(QRect* rect);
	
	//Shows text for showTime
	bool showOnly();
	
	/* Make text appear letter by letter, say for showTime, 
	and disappear in the same manner*/ 
	bool stepInAndOut();
signals:
	void textShowed();	
public slots:
	void shotTimer();
	void showTimeExpired();
	
private:
	
	enum EffectState  { ESNone, ESShowing, ESStepIn,  ESStepOut };
	enum SequenceName { SNNone, SNShowOnly, SNStepInAndOut };
	void effectSequencer();
	EffectState nextState();
	void initState();
	int step_;
	bool runningEffect_, runningSequence_;
	QString text_;
	QPainter * painter_;
	QRect * rect_;
	
	SequenceName sequenceName_;
	EffectState state_;
	QQueue<EffectState> effectSequence_;
	
	QTimer timer_;
};

inline void TextEffects:: loadText(const QString& text)
{
	text_ = text;
}

inline void TextEffects:: setPainter( QPainter* painter)
{
	painter_ = painter;
}

inline void TextEffects:: setRect(QRect* rect)
{
	rect_ = rect;
}

inline void TextEffects:: setShowTime(int showTime)
{
	timer_.setInterval( showTime);
}

inline void TextEffects:: shotTimer()
{	
	timer_.start();	
}

inline void TextEffects:: showTimeExpired()
{
	runningEffect_ = false;
}

#endif
