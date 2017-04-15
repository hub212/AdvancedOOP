set(user1 schwart7)
set(user2 shlomizabari)
set(id1 "309475721")
set(id2 "301431664")
set (SOURCE_FILES
	Board.cpp
	Board.h
	BoardChecker.cpp
	BoardChecker.h
	ex1.cpp
	ex1.h
	Game.cpp
	Game.h
	IBattleshipGameAlgo.h
	Tests.cpp
	Tests.h
)

### DO NOT CHANGE FROM HERE ON ###

message("user: ${user1}, id: ${id1}")
message("user: ${user2}, id: ${id2}")
message("user: ${user3}, id: ${id3}")

set(EXE_NAME "ex1.${user1}")
add_executable( ${EXE_NAME}
	${SOURCE_FILES}
)
#BUILD_BINARY_OUTPUT_DIRECTORY
set_property(TARGET ${EXE_NAME} PROPERTY FOLDER "ex1/${user1}")
set_property(TARGET ${EXE_NAME} PROPERTY OUTPUT_NAME "${user1}")