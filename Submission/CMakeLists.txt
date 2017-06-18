#############################################################
### YOU SHOULD EDIT THE BELOW WITH YOUR DETAILS AND FILES ###
#############################################################

set(user1 shlomizabari)
set(user2 schwart7)
set(user3 edidavidov)
set(id1 "301431664")
set(id2 "309475721")
set(id3 "314292590")

# All source files needed to compile your exe
set (EXE_SOURCE_FILES
	Board.h
	BoardChecker.h
	IBattleshipGameAlgo.h
	main.h
	SingleGameManager.h
	TournamentManager.h
	Types.h
	Utils.h
	Board.cpp
	BoardChecker.cpp
	main.cpp
	SingleGameManager.cpp
	TournamentManager.cpp
	Types.cpp
	Utils.cpp
)
# All source files needed to compile your smart algorithm's .dll
set (SMART_ALGO_SOURCE_FILES
	Board.h
	IBattleshipGameAlgo.h
	main.h
	Types.h
	Utils.h
	Smart3DPlayer.h
	Smart3DPlayer.cpp
)

##################################
### DO NOT CHANGE FROM HERE ON ###
##################################

message("user: ${user1}, id: ${id1}")

add_definitions(-DUNICODE -D_UNICODE)

set(EX ex3)

# Set exe build parameters
set(EXE_NAME "${EX}.${user1}")
add_executable( ${EXE_NAME}	${EXE_SOURCE_FILES})
set_property(TARGET ${EXE_NAME} PROPERTY FOLDER "${EX}/${user1}")
set_property(TARGET ${EXE_NAME} PROPERTY OUTPUT_NAME "${user1}")

# Set smart algorithm's .dll build parameters
set(ALG3_NAME "${EX}.${user1}.smart")
add_library(${ALG3_NAME} SHARED ${SMART_ALGO_SOURCE_FILES})
set_property(TARGET ${ALG3_NAME} PROPERTY FOLDER "${EX}/${user1}")
set_property(TARGET ${ALG3_NAME} PROPERTY OUTPUT_NAME "${user1}.smart")
target_compile_definitions(${ALG3_NAME} PRIVATE -DALGO_EXPORTS) # preprocessor flag saying we are the one compiling the header now