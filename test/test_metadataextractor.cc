/*
 * Copyright (C) 2013 Canonical, Ltd.
 *
 * Authors:
 *    Jussi Pakkanen <jussi.pakkanen@canonical.com>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of version 3 of the GNU General Public License as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <mediascanner/MediaFile.hh>
#include <daemon/FileTypeDetector.hh>
#include <daemon/MetadataExtractor.hh>

#include<stdexcept>
#include<cstdio>
#include<string>
#include<gst/gst.h>
#include <gtest/gtest.h>

using namespace std;

class MetadataExtractorTest : public ::testing::Test {
 protected:
  MetadataExtractorTest() {
  }

  virtual ~MetadataExtractorTest() {
  }

  virtual void SetUp() {
  }

  virtual void TearDown() {
  }
};

TEST_F(MetadataExtractorTest, init) {
    MetadataExtractor extractor;
}

TEST_F(MetadataExtractorTest, extract) {
    MetadataExtractor e;
    string testfile = SOURCE_DIR "/media/testfile.ogg";
    MediaFile file = e.extract(testfile);

    ASSERT_EQ(file.getTitle(), "track1");
    ASSERT_EQ(file.getAuthor(), "artist1");
    ASSERT_EQ(file.getAlbum(), "album1");
    ASSERT_EQ(file.getDate(), "2013");
    ASSERT_EQ(file.getTrackNumber(), 1);
    ASSERT_EQ(file.getDuration(), 5);

    string nomediafile = SOURCE_DIR "/CMakeLists.txt";
    ASSERT_THROW(e.extract(nomediafile), runtime_error);
}

TEST_F(MetadataExtractorTest, detector) {
    FileTypeDetector d;
    string testfile = SOURCE_DIR "/media/testfile.ogg";
    string nomediafile = SOURCE_DIR "/CMakeLists.txt";
    ASSERT_EQ(d.detect(testfile), AudioMedia);
    ASSERT_EQ(d.detect("/a/non/existing/file"), UnknownMedia);
    ASSERT_EQ(d.detect(nomediafile), UnknownMedia);
}

int main(int argc, char **argv) {
    gst_init (&argc, &argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
