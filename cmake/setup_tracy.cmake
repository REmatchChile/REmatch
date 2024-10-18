include(FetchContent)

message("Fetching Tracy")

Set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare (
  tracy
  GIT_REPOSITORY https://github.com/wolfpld/tracy.git
  GIT_TAG master
  GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(tracy)
