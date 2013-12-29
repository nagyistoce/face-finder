#include "texteffects.h"
#include <QPainter>
#include <QRect>

//
TextEffects:: TextEffects(QObject *parent) : QObject( parent)
{
	painter_ = 0;
	rect_ = 0;
	step_ = 0;
	text_ = "Load your text!";
	state_ = ESNone;
	sequenceName_ = SNNone;
	runningEffect_ = false;
	runningSequence_ = false;
	timer_.setSingleShot(true);
	timer_.setInterval(2000);
	connect( this, SIGNAL(textShowed()), this, SLOT( shotTimer()));
	connect( &timer_, SIGNAL(timeout()), this, SLOT( showTimeExpired()));
}

bool TextEffects:: showOnly()
{
	if (sequenceName_ == SNNone)
	{
		sequenceName_ = SNShowOnly;
		effectSequence_.enqueue( ESShowing);
		runningSequence_ = true;				
	}	
	if (sequenceName_ == SNShowOnly)
		effectSequencer();
	return runningSequence_;	
}
bool TextEffects:: stepInAndOut()
{
	if (sequenceName_ == SNNone)
	{
		sequenceName_ = SNStepInAndOut;
		effectSequence_.enqueue( ESStepIn);
		effectSequence_.enqueue( ESShowing);
		effectSequence_.enqueue( ESStepOut);
		runningSequence_ = true;				
	}	
	if (sequenceName_ == SNStepInAndOut)
		effectSequencer();
	return runningSequence_;	
}

void TextEffects:: effectSequencer()
{
	if (!runningEffect_)
	{
		state_ = nextState();
		initState();
	}
	
	switch (state_)
	{
		case ESShowing:
			painter_->drawText( *rect_, 
			                    Qt::AlignVCenter + Qt::AlignHCenter,
			                    text_);
			break;
		case ESStepIn:
			if (++step_ <= text_.length())
				painter_->drawText( *rect_, 
				                    Qt::AlignVCenter + Qt::AlignHCenter,
				                    text_.mid(0, step_) );
			 else runningEffect_ = false;
			 break;
		case ESStepOut:
			if (--step_ >= 0)
				painter_->drawText( *rect_, 
				                    Qt::AlignVCenter + Qt::AlignHCenter,
				                    text_.mid(0, step_) );
			else runningEffect_ = false;
			break;
		case ESNone:
		default:
			sequenceName_ = SNNone;
			runningSequence_ = false;			
	}	
}

TextEffects::EffectState TextEffects:: nextState()
{
	if (!effectSequence_.isEmpty())
		return effectSequence_.dequeue();
	else
		return ESNone;
}

void TextEffects:: initState()
{
	runningEffect_ = true;
	
	switch (state_)
	{
		case ESShowing:
			emit textShowed();
			break;
		case ESStepIn:
			step_ = 0;
			break;
		case ESStepOut:
			step_ = text_.length(); 
			break;
		case ESNone:
		default:
			runningEffect_ = false;
	}
}