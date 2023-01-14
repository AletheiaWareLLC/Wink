#include <gtest/gtest.h>

#include <Wink/semver.h>

TEST(SemVerTest, Core) {
  SemVer s1(1, 2, 3, "", "");
  ASSERT_EQ(1, s1.major());
  ASSERT_EQ(2, s1.minor());
  ASSERT_EQ(3, s1.patch());

  SemVer s2("1.2.3");
  ASSERT_EQ(1, s2.major());
  ASSERT_EQ(2, s2.minor());
  ASSERT_EQ(3, s2.patch());
}

TEST(SemVerTest, CorePrerelease) {
  SemVer s1(1, 2, 3, "alpha", "");
  ASSERT_EQ(1, s1.major());
  ASSERT_EQ(2, s1.minor());
  ASSERT_EQ(3, s1.patch());
  ASSERT_EQ("alpha", s1.prerelease());

  SemVer s2("1.2.3-alpha");
  ASSERT_EQ(1, s2.major());
  ASSERT_EQ(2, s2.minor());
  ASSERT_EQ(3, s2.patch());
  ASSERT_EQ("alpha", s2.prerelease());
}

TEST(SemVerTest, CoreBuild) {
  SemVer s1(1, 2, 3, "", "2023.1.11");
  ASSERT_EQ(1, s1.major());
  ASSERT_EQ(2, s1.minor());
  ASSERT_EQ(3, s1.patch());
  ASSERT_EQ("2023.1.11", s1.build());

  SemVer s2("1.2.3+2023.1.11");
  ASSERT_EQ(1, s2.major());
  ASSERT_EQ(2, s2.minor());
  ASSERT_EQ(3, s2.patch());
  ASSERT_EQ("2023.1.11", s2.build());
}

TEST(SemVerTest, CorePrereleaseBuild) {
  SemVer s1(1, 2, 3, "alpha", "2023.1.11");
  ASSERT_EQ(1, s1.major());
  ASSERT_EQ(2, s1.minor());
  ASSERT_EQ(3, s1.patch());
  ASSERT_EQ("alpha", s1.prerelease());
  ASSERT_EQ("2023.1.11", s1.build());

  SemVer s2("1.2.3-alpha+2023.1.11");
  ASSERT_EQ(1, s2.major());
  ASSERT_EQ(2, s2.minor());
  ASSERT_EQ(3, s2.patch());
  ASSERT_EQ("alpha", s2.prerelease());
  ASSERT_EQ("2023.1.11", s2.build());
}

TEST(SemVerTest, Stream) {
  SemVer s1(1, 2, 3, "alpha", "2023.1.11");
  std::ostringstream oss;
  oss << s1;

  SemVer s2;
  std::istringstream iss(oss.str());
  iss >> s2;

  ASSERT_EQ(1, s2.major());
  ASSERT_EQ(2, s2.minor());
  ASSERT_EQ(3, s2.patch());
  ASSERT_EQ("alpha", s2.prerelease());
  ASSERT_EQ("2023.1.11", s2.build());
}

TEST(SemVerTest, Compare) {
  ASSERT_TRUE(SemVer("1.0.0") < SemVer("2.0.0"));
  ASSERT_TRUE(SemVer("1.0.0") < SemVer("1.1.0"));
  ASSERT_TRUE(SemVer("1.0.0") < SemVer("1.0.1"));
  ASSERT_TRUE(SemVer("1.0.0-alpha") < SemVer("1.0.0"));
  ASSERT_TRUE(SemVer("1.0.0-alpha") < SemVer("1.0.0-alpha.1"));
  ASSERT_TRUE(SemVer("1.0.0-alpha.1") < SemVer("1.0.0-alpha.beta"));
  ASSERT_TRUE(SemVer("1.0.0-alpha.beta") < SemVer("1.0.0-beta"));
  ASSERT_TRUE(SemVer("1.0.0-beta") < SemVer("1.0.0-beta.2"));
  ASSERT_TRUE(SemVer("1.0.0-beta.2") < SemVer("1.0.0-beta.11"));
  ASSERT_TRUE(SemVer("1.0.0-beta.1") < SemVer("1.0.0-rc1"));
  ASSERT_TRUE(SemVer("1.0.0-rc1") < SemVer("1.0.0"));
  ASSERT_TRUE(SemVer("1.0.0-alpha+2023.1.11") <
              SemVer("1.0.0-alpha+2023.1.12"));
  ASSERT_TRUE(SemVer("1.0.0-alpha+2023.x") < SemVer("1.0.0-alpha+2023.y"));
  ASSERT_TRUE(SemVer("1.0.0-alpha+2023.1") < SemVer("1.0.0-alpha+2023.x"));
  ASSERT_TRUE(SemVer("1.0.0-alpha+2023.1") < SemVer("1.0.0-alpha+2023.1.11"));
}
