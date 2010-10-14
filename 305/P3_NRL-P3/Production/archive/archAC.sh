#! /bin/csh -f
#
# This is an example script.
# Set PROJECT and uncomment each command to archive that data type.

###############
#   Project   #
###############
set PROJECT = "TPARC"
set PROJNUM = "305";

### ADS - Done
#/net/work/bin/scripts/mass_store/archAC/archAC.py ADS /scr/raf2/Raw_Data/$PROJECT ads$ RAF

### CHAT - does this exist?
#/net/work/bin/scripts/mass_store/archAC/archAC.py CHAT -t /scr/raf2/Raw_Data/$PROJECT/Chat log RAF

# UNALTERED LRT - DONE
#/net/work/bin/scripts/mass_store/archAC/archAC.py UNALTERED/LRT /jnet/productiondata ${PROJNUM}.f...nc RAF
#/net/work/bin/scripts/mass_store/archAC/archAC.py UNALTERED/LRT /jnet/productiondata ${PROJNUM}.f....nc RAF

# LRT - done
#/net/work/bin/scripts/mass_store/archAC/archAC.py LRT /scr/raf2/Prod_Data/$PROJECT .nc ATDdata
