#! /bin/bash
#                          __  __            _
#                       ___\ \/ /_ __   __ _| |_
#                      / _ \\  /| '_ \ / _` | __|
#                     |  __//  \| |_) | (_| | |_
#                      \___/_/\_\ .__/ \__,_|\__|
#                               |_| XML parser
#
# Copyright (c) 2017-2024 Sebastian Pipping <sebastian@pipping.org>
# Copyright (c) 2017      Rolf Eike Beer <eike@sf-mail.de>
# Copyright (c) 2019      Mohammed Khajapasha <mohammed.khajapasha@intel.com>
# Copyright (c) 2019      Manish, Kumar <manish3.kumar@intel.com>
# Copyright (c) 2019      Philippe Antoine <contact@catenacyber.fr>
# Copyright (c) 2024      Dag-Erling Smørgrav <des@des.dev>
# Licensed under the MIT license:
#
# Permission is  hereby granted,  free of charge,  to any  person obtaining
# a  copy  of  this  software   and  associated  documentation  files  (the
# "Software"),  to  deal in  the  Software  without restriction,  including
# without  limitation the  rights  to use,  copy,  modify, merge,  publish,
# distribute, sublicense, and/or sell copies of the Software, and to permit
# persons  to whom  the Software  is  furnished to  do so,  subject to  the
# following conditions:
#
# The above copyright  notice and this permission notice  shall be included
# in all copies or substantial portions of the Software.
#
# THE  SOFTWARE  IS  PROVIDED  "AS  IS",  WITHOUT  WARRANTY  OF  ANY  KIND,
# EXPRESS  OR IMPLIED,  INCLUDING  BUT  NOT LIMITED  TO  THE WARRANTIES  OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN
# NO EVENT SHALL THE AUTHORS OR  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
# DAMAGES OR  OTHER LIABILITY, WHETHER  IN AN  ACTION OF CONTRACT,  TORT OR
# OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
# USE OR OTHER DEALINGS IN THE SOFTWARE.

set -e

if [[ ${RUNNER_OS} = macOS ]]; then
    latest_brew_python3_bin="$(ls -1d /usr/local/Cellar/python/3.*/bin | sort -n | tail -n1)"
    export PATH="${latest_brew_python3_bin}${PATH:+:}${PATH}"
elif [[ ${RUNNER_OS} = Linux ]]; then
    export PATH="/usr/lib/llvm-19/bin:${PATH}"
else
    echo "Unsupported RUNNER_OS \"${RUNNER_OS}\"." >&2
    exit 1
fi

echo "New \${PATH}:"
tr : '\n' <<<"${PATH}" | sed 's,^,- ,'
echo

PS4='# '
set -x

cd expat
./buildconf.sh

if [[ ${MODE} = distcheck ]]; then
    ./configure ${CONFIGURE_ARGS}
    make distcheck
elif [[ ${MODE} = cmake-oos ]]; then
    mkdir build
    cd build
    cmake ${CMAKE_ARGS} ..
    make VERBOSE=1 CTEST_OUTPUT_ON_FAILURE=1 all test
    make DESTDIR="${PWD}"/ROOT install
    find ROOT | cut -c 6- | sort
elif [[ ${MODE} = coverage-sh ]]; then
    ./coverage.sh
else
    ./qa.sh ${CMAKE_ARGS}
fi
