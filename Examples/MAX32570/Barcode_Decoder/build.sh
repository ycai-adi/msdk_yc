#!/bin/bash
#
#

BOARD=""
EXAMPLE_MODE=""
MAKEGOAL="all"

while [[ $# -gt 0 ]]; do
	case $1 in
	--goal)
		MAKEGOAL="$2"
		shift # past argument
		shift # past value
		;;
	--board)
		BOARD="$2"
		shift # past argument
		shift # past value
		;;
	--mode)
		EXAMPLE_MODE="$2"
		shift # past argument
		shift # past value
		;;
	-*|--*)
		echo "Unknown option $1"
		exit 1
		;;
	*)
		shift # past argument
		;;
	esac
done

if [ "$BOARD" == "" ]; then
	echo "INFO: Default board will be used"
fi

# reset folder
git checkout ./


case ${EXAMPLE_MODE} in
"CAM_OV7670_DMA_ON")
	# Set camera as CAM_OV7670
	sed -ri 's/#define[ ]*ACTIVE_CAMERA /#define ACTIVE_CAMERA CAM_OV7670 \/\//g' ./include/camera.h
	# DMA ON
	sed -ri 's/#define[ ]*USE_DMA /#define USE_DMA 1 \/\//g' ./src/camera.c
	;;
"CAM_OV7670_DMA_OFF")
	# Set camera as CAM_OV7670
	sed -ri 's/#define[ ]*ACTIVE_CAMERA /#define ACTIVE_CAMERA CAM_OV7670 \/\//g' ./include/camera.h
	# DMA OFF
	sed -ri 's/#define[ ]*USE_DMA /#define USE_DMA 0 \/\//g' ./src/camera.c
	;;


"CAM_OV7725_DMA_ON")
	# Set camera as CAM_OV7725
	sed -ri 's/#define[ ]*ACTIVE_CAMERA /#define ACTIVE_CAMERA CAM_OV7725 \/\//g' ./include/camera.h
	# DMA ON
	sed -ri 's/#define[ ]*USE_DMA /#define USE_DMA 1 \/\//g' ./src/camera.c
	;;
"CAM_OV7725_DMA_OFF")
	# Set camera as CAM_OV7725
	sed -ri 's/#define[ ]*ACTIVE_CAMERA /#define ACTIVE_CAMERA CAM_OV7725 \/\//g' ./include/camera.h
	# DMA OFF
	sed -ri 's/#define[ ]*USE_DMA /#define USE_DMA 0 \/\//g' ./src/camera.c
	;;

"CAM_OV2640_DMA_ON")
	# Set camera as CAM_OV2640
	sed -ri 's/#define[ ]*ACTIVE_CAMERA /#define ACTIVE_CAMERA CAM_OV2640 \/\//g' ./include/camera.h
	# DMA ON
	sed -ri 's/#define[ ]*USE_DMA /#define USE_DMA 1 \/\//g' ./src/camera.c
	;;
"CAM_OV2640_DMA_OFF")
	# Set camera as CAM_OV2640
	sed -ri 's/#define[ ]*ACTIVE_CAMERA /#define ACTIVE_CAMERA CAM_OV2640 \/\//g' ./include/camera.h
	# DMA OFF
	sed -ri 's/#define[ ]*USE_DMA /#define USE_DMA 0 \/\//g' ./src/camera.c
	;;
	
*)
	;;
esac


make ${MAKEGOAL} -j 8 BOARD=${BOARD} 
