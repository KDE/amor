//---------------------------------------------------------------------------
//
// amoranim.h
//
// Copyright (c) 1999 Martin R. Jones <mjones@kde.org>
//

#include <stdlib.h>
#include <kconfig.h>
#include <kapp.h>
#include <kglobal.h>
#include <kstddirs.h>
#include "amoranim.h"
#include "amorpm.h"

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
        pixmap = AmorPixmapManager::manager()->pixmap(*mSequence.at(mCurrent));

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
    mSequence = config.readListEntry("Sequence");
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
    QStrList list;
    int entries = config.readListEntry("Delay",list);
    mDelay.resize(frames);
    for (int i = 0; i < entries && i < frames; i++)
        mDelay[i] = atoi(list.at(i));

    // Read the distance to move between frames and calculate the total
    // distance that this aniamtion moves from its starting position.
    entries = config.readListEntry("Movement",list);
    mMovement.resize(frames);
    for (int i = 0; i < entries; i++)
    {
        mMovement[i] = atoi(list.at(i));
        mTotalMovement += mMovement[i];
    }

    // Read the hotspot for each frame.
    entries = config.readListEntry("HotspotX",list);
    mHotspot.resize(frames);
    for (int i = 0; i < entries && i < frames; i++)
        mHotspot[i].setX(atoi(list.at(i)));

    entries = config.readListEntry("HotspotY",list);
    for (int i = 0; i < entries && i < frames; i++)
        mHotspot[i].setY(atoi(list.at(i)));

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
    mAnimations.setAutoDelete(true);
}

//---------------------------------------------------------------------------
//
AmorThemeManager::~AmorThemeManager()
{
    if (mConfig)
        delete mConfig;
}

//---------------------------------------------------------------------------
//
bool AmorThemeManager::setTheme(const QString & file)
{
    mPath = locate("appdata", file);

    if (mConfig)
        delete mConfig;

    mConfig = new KSimpleConfig(mPath, true);
    mConfig->setGroup("Config");

    // Get the directory where the pixmaps are stored and tell the
    // pixmap manager.
    QString pixmapPath = mConfig->readEntry("PixmapPath");
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
        mPath.truncate(mPath.findRev('/')+1);
        mPath += pixmapPath;
    }

    mStatic = mConfig->readBoolEntry("Static", false);

    mMaximumSize.setWidth(0);
    mMaximumSize.setHeight(0);

    mAnimations.clear();

    return true;
}

//---------------------------------------------------------------------------
//
// Select an animimation randomly from a group
//
AmorAnim *AmorThemeManager::random(const QString & group)
{
    QString grp( group );

    if (mStatic)
	grp = "Base";

    AmorAnimationGroup *animGroup = mAnimations.find(grp);

    if (animGroup) {
	int idx = kapp->random()%animGroup->count();
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
    animList->setAutoDelete(true);

    // Read the list of available animations.
    mConfig->setGroup("Config");
    QStrList list;
    int entries = mConfig->readListEntry(seq, list);

    // Read each individual animation
    for (int i = 0; i < entries; i++)
    {
        mConfig->setGroup(list.at(i));
        AmorAnim *anim = new AmorAnim(*mConfig);
        animList->append(anim);
        mMaximumSize = mMaximumSize.expandedTo(anim->maximumSize());
    }

    // If no animations were available for this group, just add the base anim
    if (entries == 0)
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
    if (entries == 0)
        return false;

    mAnimations.insert(seq, animList);

    return true;
}

