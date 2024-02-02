include(FetchContent)

message("Fetching Tracy")

Set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare (
  tracy
  GIT_REPOSITORY https://github.com/wolfpld/tracy.git
  GIT_TAG master
  GIT_SHALLOW TRUE
)

if (ENABLE_PROFILING)
  set(TRACY_ENABLE ON CACHE BOOL "Enable Tracy" FORCE)
  set(TRACY_NO_EXIT ON CACHE BOOL "No Exit Tracy" FORCE)
  set(TRACY_DEBUGINFOD ON CACHE BOOL "Use DEBUGINFOD Tracy" FORCE)
else()
  set(TRACY_ENABLE OFF CACHE BOOL "Disable Tracy" FORCE)
endif()

FetchContent_MakeAvailable(tracy)
