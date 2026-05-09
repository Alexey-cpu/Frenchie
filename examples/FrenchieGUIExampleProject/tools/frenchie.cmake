include(FetchContent)

# load Frenchie from remote
FetchContent_Declare(
    Frenchie
    GIT_REPOSITORY "https://github.com/Alexey-cpu/Frenchie.git"
    GIT_TAG "origin/v1/release"
    OVERRIDE_FIND_PACKAGE
	GIT_SHALLOW TRUE)

FetchContent_MakeAvailable(Frenchie)