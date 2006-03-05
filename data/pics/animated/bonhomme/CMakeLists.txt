kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )


########### install files ###############

install_files( ${DATA_INSTALL_DIR}/amor/pics/animated/bonhomme FILES b1.png              b1-r.png              b2.png              b2-r.png              b3.png              b3-r.png              b4.png              b4-r.png              bcol1.png              bcol2.png              bdrib1.png              bdrib2.png              bdrib3.png              bdrib4.png              bdrib5.png              bdrib6.png              bdrib7.png              bdrop.png              b-f2.png              b-f.png              bgo1.png              bgo2.png              bgo3.png              bgo4.png              bgo5.png              bgo6.png              bgo7.png              bh1.png              bpof1.png              bpof2.png              bpof3.png              bpof4.png              bpof5.png              bpof6.png              sa1.png               sa2a.png              sa2b.png              sa2c.png              sa2.png              salut1.png              salut.png )

kde4_footer()



#original Makefile.am contents follow:

## Add all your pic files here
#amor_DATA = b1.png \
#            b1-r.png \
#            b2.png \
#            b2-r.png \
#            b3.png \
#            b3-r.png \
#            b4.png \
#            b4-r.png \
#            bcol1.png \
#            bcol2.png \
#            bdrib1.png \
#            bdrib2.png \
#            bdrib3.png \
#            bdrib4.png \
#            bdrib5.png \
#            bdrib6.png \
#            bdrib7.png \
#            bdrop.png \
#            b-f2.png \
#            b-f.png \
#            bgo1.png \
#            bgo2.png \
#            bgo3.png \
#            bgo4.png \
#            bgo5.png \
#            bgo6.png \
#            bgo7.png \
#            bh1.png \
#            bpof1.png \
#            bpof2.png \
#            bpof3.png \
#            bpof4.png \
#            bpof5.png \
#            bpof6.png \
#            sa1.png  \
#            sa2a.png \
#            sa2b.png \
#            sa2c.png \
#            sa2.png \
#            salut1.png \
#            salut.png
#
#
## This is where it will all be installed
#amordir = $(kde_datadir)/amor/pics/animated/bonhomme
