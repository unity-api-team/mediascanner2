/*
 * Copyright (C) 2013-2014 Canonical, Ltd.
 *
 * Authors:
 *    Jussi Pakkanen <jussi.pakkanen@canonical.com>
 *    James Henstridge <james.henstridge@canonical.com>
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
#include <extractor/DetectedFile.hh>
#include <extractor/MetadataExtractor.hh>

#include "test_config.h"

#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <gio/gio.h>
#include <gtest/gtest.h>

using namespace std;
using namespace mediascanner;

class MetadataExtractorTest : public ::testing::Test {
protected:
    MetadataExtractorTest() :
        test_dbus(nullptr, g_object_unref),
        session_bus(nullptr, g_object_unref) {
    }

    virtual ~MetadataExtractorTest() {
    }

    virtual void SetUp() override{
        test_dbus.reset(g_test_dbus_new(G_TEST_DBUS_NONE));
        g_test_dbus_add_service_dir(test_dbus.get(), TEST_DIR "/services");
        g_test_dbus_up(test_dbus.get());

        GError *error = nullptr;
        session_bus.reset(g_bus_get_sync(G_BUS_TYPE_SESSION, nullptr, &error));
        if (!session_bus) {
            std::string errortxt(error->message);
            g_error_free(error);
            throw std::runtime_error(
                std::string("Failed to connect to session bus: ") + errortxt);
        }
    }

    virtual void TearDown() override {
        session_bus.reset();
        g_test_dbus_down(test_dbus.get());
        test_dbus.reset();
    }

    unique_ptr<GTestDBus,decltype(&g_object_unref)> test_dbus;
    unique_ptr<GDBusConnection,decltype(&g_object_unref)> session_bus;
};


TEST_F(MetadataExtractorTest, init) {
    MetadataExtractor extractor(session_bus.get());
}

TEST_F(MetadataExtractorTest, detect_audio) {
    MetadataExtractor e(session_bus.get());
    string testfile = SOURCE_DIR "/media/testfile.ogg";
    DetectedFile d = e.detect(testfile);
    EXPECT_NE(d.etag, "");
    EXPECT_EQ(d.content_type, "audio/ogg");
    EXPECT_EQ(d.type, AudioMedia);
}

TEST_F(MetadataExtractorTest, detect_video) {
    MetadataExtractor e(session_bus.get());
    string testfile = SOURCE_DIR "/media/testvideo_480p.ogv";
    DetectedFile d = e.detect(testfile);
    EXPECT_NE(d.etag, "");
    EXPECT_EQ(d.content_type, "video/ogg");
    EXPECT_EQ(d.type, VideoMedia);
}

TEST_F(MetadataExtractorTest, detect_notmedia) {
    MetadataExtractor e(session_bus.get());
    string testfile = SOURCE_DIR "/CMakeLists.txt";
    EXPECT_THROW(e.detect(testfile), runtime_error);
}

TEST_F(MetadataExtractorTest, extract) {
    MetadataExtractor e(session_bus.get());
    string testfile = SOURCE_DIR "/media/testfile.ogg";
    MediaFile file = e.extract(e.detect(testfile));

    EXPECT_EQ(file.getType(), AudioMedia);
    EXPECT_EQ(file.getTitle(), "track1");
    EXPECT_EQ(file.getAuthor(), "artist1");
    EXPECT_EQ(file.getAlbum(), "album1");
    EXPECT_EQ(file.getDate(), "2013");
    EXPECT_EQ(file.getTrackNumber(), 1);
    EXPECT_EQ(file.getDuration(), 5);
}

TEST_F(MetadataExtractorTest, extract_video) {
    MetadataExtractor e(session_bus.get());

    MediaFile file = e.extract(e.detect(SOURCE_DIR "/media/testvideo_480p.ogv"));
    EXPECT_EQ(file.getType(), VideoMedia);
    EXPECT_EQ(file.getDuration(), 1);
    EXPECT_EQ(file.getWidth(), 854);
    EXPECT_EQ(file.getHeight(), 480);

    file = e.extract(e.detect(SOURCE_DIR "/media/testvideo_720p.ogv"));
    EXPECT_EQ(file.getType(), VideoMedia);
    EXPECT_EQ(file.getDuration(), 1);
    EXPECT_EQ(file.getWidth(), 1280);
    EXPECT_EQ(file.getHeight(), 720);

    file = e.extract(e.detect(SOURCE_DIR "/media/testvideo_1080p.ogv"));
    EXPECT_EQ(file.getType(), VideoMedia);
    EXPECT_EQ(file.getDuration(), 1);
    EXPECT_EQ(file.getWidth(), 1920);
    EXPECT_EQ(file.getHeight(), 1080);
}

TEST_F(MetadataExtractorTest, extract_photo) {
    MetadataExtractor e(session_bus.get());

    // An landscape image that should be rotated to portrait
    MediaFile file = e.extract(e.detect(SOURCE_DIR "/media/image1.jpg"));
    EXPECT_EQ(ImageMedia, file.getType());
    EXPECT_EQ(2848, file.getWidth());
    EXPECT_EQ(4272, file.getHeight());
    EXPECT_EQ("2013-01-04T08:25:46", file.getDate());
    EXPECT_DOUBLE_EQ(-28.249409333333336, file.getLatitude());
    EXPECT_DOUBLE_EQ(153.150774, file.getLongitude());

    // A landscape image without rotation.
    file = e.extract(e.detect(SOURCE_DIR "/media/image2.jpg"));
    EXPECT_EQ(ImageMedia, file.getType());
    EXPECT_EQ(4272, file.getWidth());
    EXPECT_EQ(2848, file.getHeight());
    EXPECT_EQ("2013-01-04T09:52:27", file.getDate());
    EXPECT_DOUBLE_EQ(-28.259611, file.getLatitude());
    EXPECT_DOUBLE_EQ(153.1727346, file.getLongitude());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
