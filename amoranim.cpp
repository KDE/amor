/* amoranim.cpp
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
#include <kconfigbase.h>
#include <krandom.h>
#include <ksimpleconfig.h>
#include <kstandarddirs.h>
#include "amoranim.h"
#include "amorpm.h"
#include <QPixmap>

//---------------------------------------------------------------------------
//
// Constructor
//
AmorAnim::AmorAnim(KConfigBase &config)
    : mMaximumSize(0, 0)
{
    mCurrent = 0;
    mTotalMovement = 0;
    readConfig(config);
}

//---------------------------------------------------------------------------
//
// Destructor
//
AmorAnim::~AmorAnim()
{
}

//---------------------------------------------------------------------------
//
// Get the Pixmap for the current frame.
//
const QPixmap *AmorAnim::frame()
{
    const QPixmap *pixmap = 0;

    if (validFrame())
        pixmap = AmorPixmapManager::manager()->pixmap(mSequence.at(mCurrent));

    return pixmap;
}

//---------------------------------------------------------------------------
//
// Read a single animation's parameters.  The config class should already
// have its group set to the animation that is to be read.
//
void AmorAnim::readConfig(KConfigBase &config)
{
    // Read the list of frames to display and load them into the pixmap
    // manager.
    mSequence = config.readEntry("Sequence",QStringList());
    int frames = mSequence.count();
    for ( QStringList::Iterator it = mSequence.begin();
          it != mSequence.end();
          ++it )
    {
        const QPixmap *pixmap =
                        AmorPixmapManager::manager()->load(*it);
        if (pixmap)
            mMaximumSize = mMaximumSize.expandedTo(pixmap->size());
    }

    // Read the delays between frames.
    QStringList list;
    list = config.readEntry("Delay",QStringList());
	mDelay.resize(list.count());
    for (int i = 0; i < list.count() && i < frames; i++)
        mDelay[i] = list.at(i).toInt();

    // Read the distance to move between frames and calculate the total
    // distance that this aniamtion moves from its starting position.
    list = config.readEntry("Movement",QStringList());
    mMovement.resize(frames);
    for (int i = 0; i < list.count() && i < frames; i++)
    {
        mMovement[i] = list.at(i).toInt();
        mTotalMovement += mMovement[i];
    }

    // Read the hotspot for each frame.
    QStringList entries = config.readEntry("HotspotX",QStringList());
    mHotspot.resize(frames);
    for (int i = 0; i < entries.count() && i < frames; i++)
        mHotspot[i].setX(entries.at(i).toInt());

    entries = config.readEntry("HotspotY",QStringList());
    for (int i = 0; i < entries.count() && i < frames; i++)
        mHotspot[i].setY(entries.at(i).toInt());

    // Add the overlap of the last frame to the total movement.
    const QPoint &lastHotspot = mHotspot[mHotspot.size()-1];
    if (mTotalMovement > 0)
    {
        const QPixmap *lastFrame =
                    AmorPixmapManager::manager()->pixmap(mSequence.last());
        if (lastFrame)
        {
            mTotalMovement += (lastFrame->width() - lastHotspot.x());
        }
    }
    else if (mTotalMovement < 0)
    {
        mTotalMovement -= lastHotspot.x();
    }
}

//===========================================================================

AmorThemeManager::AmorThemeManager()
    : mMaximumSize(0, 0)
{
    mConfig = 0;
}

//---------------------------------------------------------------------------
//
AmorThemeManager::~AmorThemeManager()
{
    qDeleteAll(mAnimations);
    delete mConfig;
}

//---------------------------------------------------------------------------
//
bool AmorThemeManager::setTheme(const QString & file)
{
    mPath = KStandardDirs::locate("appdata", file);

    delete mConfig;

    mConfig = new KSimpleConfig(mPath, true);
    mConfig->setGroup("Config");

    // Get the directory where the pixmaps are stored and tell the
    // pixmap manager.
    QString pixmapPath = mConfig->readPathEntry("PixmapPath");
    if (pixmapPath.isEmpty())
        return false;

    if (pixmapPath[0] == '/')
    {
        // absolute path to pixmaps
        mPath = pixmapPath;
    }
    else
    {
        // relative to config file.
        mPath.truncate(mPath.lastIndexOf('/')+1);
        mPath += pixmapPath;
    }

    mStatic = mConfig->readEntry("Static", QVariant(false)).toBool();

    mMaximumSize.setWidth(0);
    mMaximumSize.setHeight(0);

    qDeleteAll(mAnimations);
    mAnimations.clear();

    return true;
}

//---------------------------------------------------------------------------
//
// Select an animimation randomly from a group
//
AmorAnim *AmorThemeManager::random(const QString & group)
{
    QString grp = mStatic ? QLatin1String("Base") : group;

    QHash<QString, AmorAnimationGroup*>::const_iterator it = mAnimations.find(grp);
    AmorAnimationGroup *animGroup = it != mAnimations.end() ? *it : 0;

    if (animGroup) {
	int idx = KRandom::random()%animGroup->count();
        return animGroup->at( idx );
    }

    return 0;
}

//---------------------------------------------------------------------------
//
// Read an animation group.
//
bool AmorThemeManager::readGroup(const QString & seq)
{
    AmorPixmapManager::manager()->setPixmapDir(mPath);

    AmorAnimationGroup *animList = new AmorAnimationGroup;
#ifdef __GNUC__
#warning "kde4: fix autodelete for animList";
#endif	
	//animList->setAutoDelete(true);

    // Read the list of available animations.
    mConfig->setGroup("Config");
    QStringList list;
    list = mConfig->readEntry(seq,QStringList());

    // Read each individual animation
    for (int i = 0; i < list.count(); i++)
    {
        mConfig->setGroup(list.at(i));
        AmorAnim *anim = new AmorAnim(*mConfig);
        animList->append(anim);
        mMaximumSize = mMaximumSize.expandedTo(anim->maximumSize());
    }
	int entries = list.count();
    // If no animations were available for this group, just add the base anim
    if ( entries == 0)
    {
        mConfig->setGroup("Base");
        AmorAnim *anim = new AmorAnim(*mConfig);
        if (anim)
        {
            animList->append(anim);
            mMaximumSize = mMaximumSize.expandedTo(anim->maximumSize());
            entries++;
        }
    }

    // Couldn't read any entries at all
    if ( entries == 0)
        return false;

    mAnimations[seq] = animList;

    return true;
}

