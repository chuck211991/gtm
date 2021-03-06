#!/usr/local/bin/tcsh -f
#################################################################
#								#
# Copyright (c) 2001-2015 Fidelity National Information		#
# Services, Inc. and/or its subsidiaries. All rights reserved.	#
#								#
#	This source code contains the intellectual property	#
#	of its copyright holder(s), and is made available	#
#	under a license.  If you do not know the terms of	#
#	the license, please stop and do not read further.	#
#								#
#################################################################
#
# Note: This script only works when called from buildaux.csh
#
set gt_image = $1
set gt_ld_options = "$2"

echo ""
echo "############# Linking LKE ###########"
echo ""
@ buildaux_lke_status = 0
source $gtm_tools/gtm_env.csh

set aix_loadmap_option = ''
if ( $HOSTOS == "AIX") then
	set aix_loadmap_option = "-bcalls:$gtm_map/lke.loadmap -bmap:$gtm_map/lke.loadmap -bxref:$gtm_map/lke.loadmap"
endif
set echo
gt_ld $gt_ld_options $aix_loadmap_option ${gt_ld_option_output}$3/lke	-L$gtm_obj $gtm_obj/{lke,lke_cmd}.o \
		$gt_ld_sysrtns $gt_ld_options_all_exe -llke -lmumps -lgnpclient -lmumps -lgnpclient -lcmisockettcp \
		$gt_ld_extra_libs $gt_ld_syslibs >& $gtm_map/lke.map
@ exit_status = $status
unset echo
if ( $exit_status != 0  ||  ! -x $3/lke ) then
	@ buildaux_lke_status++
	echo "buildaux-E-linklke, Failed to link lke (see ${dollar_sign}gtm_map/lke.map)" \
		>> $gtm_log/error.${gtm_exe:t}.log
else if ( "ia64" == $mach_type && "hpux" == $platform_name ) then
	if ( "dbg" == $gt_image ) then
		chatr +dbg enable +as mpas $3/lke
	else
		chatr +as mpas $3/lke
	endif
endif
exit $buildaux_lke_status
