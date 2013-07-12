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

#include"SubtreeWatcher.hh"

#include<cstdio>

using namespace std;

int main(int argc, char **argv) {
    SubtreeWatcher sw;
    if(argc != 2) {
        printf("%s <subdir to watch>\n", argv[0]);
        return 1;
    }
    sw.addDir(argv[1]);
    sw.run();
    return 0;
}
