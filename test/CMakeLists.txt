rock_testsuite(NormalDepthMap_core NormalDepthMap_test.cpp TestHelper.cpp
    DEPS normal_depth_map
    LIBS ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY}
    DEPS_PKGCONFIG opencv)

rock_testsuite(NormalMapping_core NormalMapping_test.cpp TestHelper.cpp
    DEPS normal_depth_map
    LIBS ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY}
    DEPS_PKGCONFIG opencv)

rock_testsuite(MaterialProperties_core MaterialProperties_test.cpp TestHelper.cpp
    DEPS normal_depth_map
    LIBS ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY}
    DEPS_PKGCONFIG opencv)

rock_testsuite(Reverberation_core Reverberation_test.cpp TestHelper.cpp
    DEPS normal_depth_map
    LIBS ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY}
    DEPS_PKGCONFIG openscenegraph opencv)

rock_testsuite(Tools_core Tools_test.cpp TestHelper.cpp
    DEPS normal_depth_map
    LIBS ${Boost_THREAD_LIBRARY} ${Boost_SYSTEM_LIBRARY}
    DEPS_PKGCONFIG openscenegraph opencv)