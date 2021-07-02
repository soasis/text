#!/usr/bin/env python

# =============================================================================
#
# ztd.text
# Copyright © 2021 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
# Contact: opensource@soasis.org
#
# Commercial License Usage
# Licensees holding valid commercial ztd.text licenses may use this file in
# accordance with the commercial license agreement provided with the
# Software or, alternatively, in accordance with the terms contained in
# a written agreement between you and Shepherd's Oasis, LLC.
# For licensing terms and conditions see your agreement. For
# further information contact opensource@soasis.org.
#
# Apache License Version 2 Usage
# Alternatively, this file may be used under the terms of Apache License
# Version 2.0 (the "License") for non-commercial use; you may not use this
# file except in compliance with the License. You may obtain a copy of the
# License at
#
#		http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# ============================================================================>

import argparse
import os, sys
import re
import datetime as dt
from io import StringIO

description = "Converts ztd.text to a single file for convenience."

# command line parser
parser = argparse.ArgumentParser(usage='%(prog)s [options...]',
                                 description=description)
parser.add_argument(
    '--output',
    '-o',
    nargs='+',
    help=
    'name and location of where to place file (and forward declaration file)',
    metavar='file',
    default=['single/include/ztd/text.hpp'])
parser.add_argument('--input',
                    '-i',
                    help='the path to use to get to the ztd.text files',
                    metavar='path',
                    default=os.path.normpath(
                        os.path.dirname(os.path.realpath(__file__)) +
                        '/../include'))
parser.add_argument('--quiet', help='suppress all output', action='store_true')
args = parser.parse_args()

single_file = ''
forward_single_file = ''
single_file = os.path.normpath(args.output[0])

if len(args.output) > 1:
	forward_single_file = args.output[1]
else:
	a, b = os.path.splitext(single_file)
	a = os.path.dirname(single_file)
	forward_single_file = os.path.normpath(
	    os.path.join(a + '/', 'forward' + b))

single_file_dir = os.path.dirname(single_file)
forward_single_file_dir = os.path.dirname(forward_single_file)

project_path = args.input
working_dir = os.getcwd()
os.chdir(project_path)

# If the user didn't provide abztd.textute paths then construct them based on the current working dir.
if not os.path.isabs(single_file):
	single_file = os.path.join(working_dir, single_file)
	single_file_dir = os.path.join(working_dir, single_file_dir)

if not os.path.isabs(forward_single_file):
	forward_single_file = os.path.join(working_dir, forward_single_file)
	forward_single_file_dir = os.path.join(working_dir,
	                                       forward_single_file_dir)

intro = """// =============================================================================
//
// ztd.text
// Copyright © 2021 JeanHeyd "ThePhD" Meneide and Shepherd's Oasis, LLC
// Contact: opensource@soasis.org
//
// Commercial License Usage
// Licensees holding valid commercial ztd.text licenses may use this file in
// accordance with the commercial license agreement provided with the
// Software or, alternatively, in accordance with the terms contained in
// a written agreement between you and Shepherd's Oasis, LLC.
// For licensing terms and conditions see your agreement. For
// further information contact opensource@soasis.org.
//
// Apache License Version 2 Usage
// Alternatively, this file may be used under the terms of Apache License
// Version 2.0 (the "License") for non-commercial use; you may not use this
// file except in compliance with the License. You may obtain a copy of the 
// License at
//
//		http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// ============================================================================>

/* This is an automatically generated header file.
*  Generated from the ztd.text repository at {time},
*  Version {version} (Revision {revision}).
*/

#ifndef {guard}
#define {guard}

"""

includes = set([])
standard_include = re.compile(r'#include <(.*?)>')
local_include = re.compile(r'#(\s*?)include "(.*?)"')
project_include = re.compile(r'#(\s*?)include <(ztd/text/.*?)>')
pragma_once_cpp = re.compile(r'(\s*)#(\s*)pragma(\s+)once')
ifndef_cpp = re.compile(r'#ifndef ZTD_TEXT_.*?_HPP')
define_cpp = re.compile(r'#define ZTD_TEXT_.*?_HPP')
endif_cpp = re.compile(r'#endif // ZTD_TEXT_.*?_HPP')
forward_cpp = re.compile(r'ZTD_TEXT_FORWARD_HPP')
forward_detail_cpp = re.compile(r'ZTD_TEXT_FORWARD_DETAIL_HPP')


def get_include(line, base_path):
	local_match = local_include.match(line)
	if local_match:
		# local include found
		full_path = os.path.normpath(
		    os.path.join(base_path,
		                 local_match.group(2))).replace('\\', '/')
		return full_path
	project_match = project_include.match(line)
	if project_match:
		# project include found
		full_path = os.path.normpath(
		    os.path.join(project_path,
		                 project_match.group(2))).replace('\\', '/')
		return full_path
	return None


def is_include_guard(line):
	is_regular_guard = ifndef_cpp.match(line) or define_cpp.match(
	    line) or endif_cpp.match(line) or pragma_once_cpp.match(line)
	if is_regular_guard:
		return not forward_cpp.search(
		    line) and not forward_detail_cpp.search(line)
	return is_regular_guard


def get_revision():
	return os.popen('git rev-parse --short HEAD').read().strip()


def get_version():
	return os.popen('git describe --tags --abbrev=0').read().strip()


def process_file(filename, out):
	global includes
	filename = os.path.normpath(filename)
	relativefilename = filename.replace(project_path + os.sep,
	                                    "").replace("\\", "/")

	if filename in includes:
		return

	includes.add(filename)

	if not args.quiet:
		print('processing {}'.format(filename))

	out.write('// beginning of {}\n\n'.format(relativefilename))
	empty_line_state = True

	with open(filename, 'r', encoding='utf-8') as f:
		for line in f:
			# skip comments
			if line.startswith('//'):
				continue

			# skip include guard non-sense
			if is_include_guard(line):
				continue

			# get relative directory
			base_path = os.path.dirname(filename)

			# check if it's a standard file
			std = standard_include.search(line)
			if std:
				std_file = os.path.join('std', std.group(0))
				#if std_file in includes:
				#	continue
				includes.add(std_file)

			# see if it's an include file
			name = get_include(line, base_path)

			if name:
				process_file(name, out)
				continue

			empty_line = len(line.strip()) == 0

			if empty_line and empty_line_state:
				continue

			empty_line_state = empty_line

			# line is fine
			out.write(line)

	out.write('// end of {}\n\n'.format(relativefilename))


version = get_version()
revision = get_revision()
if not version:
	version = "Tip"
include_guard = 'ZTD_TEXT_SINGLE_INCLUDE_HPP'
forward_include_guard = 'ZTD_TEXT_SINGLE_INCLUDE_FORWARD_HPP'

processed_files = [
    os.path.join(project_path, x) for x in ['ztd/text.hpp']
]
forward_processed_files = [
    os.path.join(project_path, x) for x in ['ztd/text/forward.hpp']
]
result = ''
forward_result = ''

if not args.quiet:
	print('Current version: {version} (revision {revision})'.format(
	    version=version, revision=revision))
	print('Project path: {}\n'.format(project_path))
	print('Creating single header for ztd.text')

ss = StringIO()
ss.write(
    intro.format(time=dt.datetime.utcnow(),
                 revision=revision,
                 version=version,
                 guard=include_guard))
for processed_file in processed_files:
	process_file(processed_file, ss)

ss.write('#endif // {}\n'.format(include_guard))
result = ss.getvalue()
ss.close()

if not args.quiet:
	print('finished creating single header for ztd.text\n')

if not args.quiet:
	print('Creating single forward declaration header for ztd.text')

includes = set([])
forward_ss = StringIO()
forward_ss.write(
    intro.format(time=dt.datetime.utcnow(),
                 revision=revision,
                 version=version,
                 guard=forward_include_guard))
for forward_processed_file in forward_processed_files:
	process_file(forward_processed_file, forward_ss)

forward_ss.write('#endif // {}\n'.format(forward_include_guard))
forward_result = forward_ss.getvalue()
forward_ss.close()

if not args.quiet:
	print(
	    'finished creating single forward declaration header for ztd.text\n')

# Create the output directories if they don't already exist.
os.makedirs(single_file_dir, exist_ok=True)
os.makedirs(forward_single_file_dir, exist_ok=True)

with open(single_file, 'w', encoding='utf-8') as f:
	if not args.quiet:
		print('writing {}...'.format(single_file))
	f.write(result)

with open(forward_single_file, 'w', encoding='utf-8') as f:
	if not args.quiet:
		print('writing {}...'.format(forward_single_file))
	f.write(forward_result)
