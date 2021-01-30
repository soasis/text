# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
import os
import sys
import subprocess
# sys.path.insert(0, os.path.abspath('.'))

# -- Project information -----------------------------------------------------

project = 'ztd.text'
copyright = "2021, JeanHeyd Meneide & Shepherd's Oasis, LLC"
author = "JeanHeyd Meneide & Shepherd's Oasis, LLC"

# The full version, including alpha/beta/rc tags
#
release = '0.0.0'

# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
#
extensions = ['breathe']

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
#
exclude_patterns = []

# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'sphinx_rtd_theme'

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
#
html_static_path = []

# Add any extra paths that contain custom files (such as robots.txt or
# .htaccess) here, relative to this directory. These files are copied
# directly to the root of the documentation.
#
html_extra_path = []

# Breathe Configuration
#
breathe_projects = {}
breathe_default_project = "ztd.text"

# ReadTheDocs Build Help
#
import subprocess, sys


def run_cmake_doxygen():
	"""Run the cmake command to get the doxygen sources"""

	# Make sure the directory exists
	cmake_dir = os.path.join(os.getcwd(), '_build/cmake-build')
	xml_dir = os.path.join(cmake_dir, 'documentation/doxygen/xml')
	os.makedirs(cmake_dir, exist_ok=True)
	os.makedirs(xml_dir, exist_ok=True)
	print("[conf.py] CMake Directory: %s" % cmake_dir)
	print("[conf.py] XML Directory: %s" % xml_dir)

	try:
		retcode = subprocess.call(
		    "cmake -DZTD_TEXT_DOCUMENTATION:BOOL=TRUE -DZTD_TEXT_DOCUMENTATION_NO_SPHINX:BOOL=TRUE ../../../..",
		    shell=True,
		    cwd=cmake_dir)
	except OSError as e:
		sys.stderr.write("cmake generation execution failed: %s\n" % e)
		return

	try:
		retcode = subprocess.call("cmake --build .",
		                          shell=True,
		                          cwd=cmake_dir)
	except OSError as e:
		sys.stderr.write("cmake generation execution failed: %s\n" % e)
		return

	breathe_projects["ztd.text"] = xml_dir


def generate_doxygen_xml(app):
	"""Run the doxygen make commands if we're on the ReadTheDocs server"""

	read_the_docs_build = os.environ.get('READTHEDOCS', None) == 'True'

	if read_the_docs_build:
		run_cmake_doxygen()


def setup(app):

	# Add hook for building doxygen xml when needed
	app.connect("builder-inited", generate_doxygen_xml)
