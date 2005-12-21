/* amoranim.h
**
** Copyright (c) 1999 Martin R. Jones <mjones@kde.org>
**
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program in a file called COPYING; if not, write to
** the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
** MA 02110-1301, USA.
*/

/*
** Bug reports and questions can be sent to kde-devel@kde.org
*/
#ifndef AMORANIM_H 
#define AMORANIM_H 

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include <stdlib.h>
#include <unistd.h>

#include <q3dict.h>
#include <qstringlist.h>
//Added by qt3to4:
#include <QPixmap>
#include <QVector>
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
    int   mCurrent;        // current frame in sequence
    QStringList    mSequence;       // sequence of images to display
    QVector<int>    mDelay;          // delay between frames
    QVector<QPoint> mHotspot;        // the hotspot in a frame
    QVector<int>    mMovement;       // the distance to move between frames
    int            mTotalMovement;  // the total distance this animation moves
    QSize          mMaximumSize;    // the maximum size of any frame
};

//---------------------------------------------------------------------------
typedef QList<AmorAnim*> AmorAnimationGroup;

//---------------------------------------------------------------------------
//
// AmorThemeManager maintains an animation theme
//
class AmorThemeManager
{
public:
    AmorThemeManager();
    virtual ~AmorThemeManager();

    bool setTheme(const QString & file);
    bool readGroup(const QString & seq);
    bool isStatic() const
	{ return mStatic; }

    AmorAnim *random(const QString & group);

    QSize maximumSize() const { return mMaximumSize; }

protected:
    QString           mPath;
    KSimpleConfig     *mConfig;
    QSize             mMaximumSize; // The largest pixmap used
    Q3Dict<AmorAnimationGroup> mAnimations; // list of animation groups
    bool              mStatic;	    // static image
};

#endif // AMORANIM_H 

