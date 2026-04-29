include(FetchContent)

# load SDL from remote
FetchContent_Declare(
    GLM
    GIT_REPOSITORY https://github.com/Alexey-cpu/glm.git
    GIT_TAG origin/master
    OVERRIDE_FIND_PACKAGE
    GIT_SHALLOW TRUE)

FetchContent_MakeAvailable(GLM)