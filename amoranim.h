//---------------------------------------------------------------------------
//
// amoranim.h
//
// Copyright (c) 1999 Martin R. Jones <mjones@kde.org>
//

#ifndef AMORANIM_H 
#define AMORANIM_H 

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include <stdlib.h>
#include <unistd.h>
#include <qarray.h>
#include <qdict.h>
#include <kconfigbase.h>
#include <ksimpleconfig.h>

//---------------------------------------------------------------------------
//
// AmorAnim contains the properties of a single animation
//
class AmorAnim
{
public:
    AmorAnim(KConfigBase &config);
    virtual ~AmorAnim();

    void reset()
        { mCurrent = 0; }
    bool next()
        { return (++mCurrent < mSequence.count()); }
    int frameNum() const
        { return mCurrent; }
    bool validFrame() const
        { return (mCurrent < mSequence.count()); }
    int totalMovement() const
        { return mTotalMovement; }
    QSize maximumSize() const
        { return mMaximumSize; }

    int delay() const
        { return (validFrame() ? mDelay[mCurrent] : 100); }
    QPoint hotspot() const
        { return (validFrame() ? mHotspot[mCurrent] : QPoint(16,16)); }
    int movement() const
        { return (validFrame() ? mMovement[mCurrent] : 0); }
    const QPixmap *frame();

protected:
    void readConfig(KConfigBase &config);

protected:
    unsigned int   mCurrent;        // current frame in sequence
    QStrList       mSequence;       // sequence of images to display
    QArray<int>    mDelay;          // delay between frames
    QArray<QPoint> mHotspot;        // the hotspot in a frame
    QArray<int>    mMovement;       // the distance to move between frames
    int            mTotalMovement;  // the total distance this animation moves
    QSize          mMaximumSize;    // the maximum size of any frame
};

//---------------------------------------------------------------------------
typedef QList<AmorAnim> AmorAnimationGroup;

//---------------------------------------------------------------------------
//
// AmorThemeManager maintains an animation theme
//
class AmorThemeManager
{
public:
    AmorThemeManager();
    virtual ~AmorThemeManager();

    bool setTheme(const char *file);
    bool readGroup(const char *seq);

    AmorAnim *random(const char *group);

    QSize maximumSize() const { return mMaximumSize; }

protected:
    QString           mPath;
    KSimpleConfig     *mConfig;
    QSize             mMaximumSize; // The largest pixmap used
    QDict<AmorAnimationGroup> mAnimations; // list of animation groups
};

#endif // AMORANIM_H 

