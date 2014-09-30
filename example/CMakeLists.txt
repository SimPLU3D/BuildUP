INCLUDE_DIRECTORIES(../include)

#copy test data
add_custom_target(zonetest_data
	"${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/data" "${CMAKE_CURRENT_BINARY_DIR}/data"
)


FILE(GLOB SRC_FILES *.cpp)
ADD_EXECUTABLE(zonetest ${SRC_FILES})
add_dependencies(zonetest zonetest_data)

TARGET_LINK_LIBRARIES(zonetest
	BuildUP
	${GDAL_LIBRARIES}
	${Boost_LIBRARIES}
)

IF(BUILD_WITH_OSG)
TARGET_LINK_LIBRARIES(zonetest
	BuildUP
	${GDAL_LIBRARIES}
	${Boost_LIBRARIES}
	${OPENSCENEGRAPH_LIBRARIES}
)	
ENDIF(BUILD_WITH_OSG)

#INSTALL( TARGETS zonetest DESTINATION bin )
